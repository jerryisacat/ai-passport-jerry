// main/main.c —— 固件入口：BSP bring-up 与按键分发，界面交给 jerryicat 启动器。
//
// 上游 demo_*.c 仍保留在 main/ 中作为硬件与并发参考，但默认启动界面由
// jerryicat/ 提供。按键守卫仍留在这一层，因为深睡唤醒后的"按住唤醒键"
// 是板级安全契约，而不是某个玩法的逻辑。
#include "bsp_battery.h"
#include "bsp_button.h"
#include "bsp_display.h"
#include "bsp_pins.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "jerryicat.h"
#include "key_guard.h"

static const char *TAG = "main";

#define INPUT_QUEUE_DEPTH 32
#define KEY_GUARD_MAX_US (10LL * 1000 * 1000)

typedef struct {
    bsp_btn_t btn;
    bsp_btn_ev_t event;
} input_event_t;

static QueueHandle_t s_input_queue;
static TaskHandle_t s_input_task;
static volatile bool s_input_ready;
static key_guard_t s_key_guard;

// button 回调运行在共享 esp_timer 任务：只守卫、入队，然后立刻返回。
static void on_key(bsp_btn_t btn, bsp_btn_ev_t ev, void *user) {
    (void)user;
    // 先消费守卫再检查就绪：release 可能在初始化后、输入派发就绪前到达，
    // 若在这里提前返回，守卫会一直武装到兜底 deadline。
    const bool was_armed = s_key_guard.armed;
    if (key_guard_consume(&s_key_guard, ev == BSP_BTN_RELEASE, esp_timer_get_time())) {
        if (was_armed && !s_key_guard.armed) {
            ESP_LOGI(TAG, "唤醒键已松手,按键恢复响应");
        }
        return;
    }
    if (!s_input_ready || !s_input_queue) return;
    const input_event_t input = { .btn = btn, .event = ev };
    (void)xQueueSend(s_input_queue, &input, 0);
}

static void input_task(void *arg) {
    (void)arg;
    input_event_t input;
    for (;;) {
        if (xQueueReceive(s_input_queue, &input, portMAX_DELAY) == pdTRUE) {
            jerryicat_handle_key(input.btn, input.event);
        }
    }
}

static esp_err_t input_dispatch_init(void) {
    s_input_queue = xQueueCreate(INPUT_QUEUE_DEPTH, sizeof(input_event_t));
    if (!s_input_queue) return ESP_ERR_NO_MEM;
    if (xTaskCreate(input_task, "input", 4096, NULL, 5, &s_input_task) != pdPASS) {
        vQueueDelete(s_input_queue);
        s_input_queue = NULL;
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static void input_dispatch_deinit(void) {
    s_input_ready = false;
    if (s_input_task) {
        vTaskDelete(s_input_task);
        s_input_task = NULL;
    }
    if (s_input_queue) {
        vQueueDelete(s_input_queue);
        s_input_queue = NULL;
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "jerryicat firmware boot");

    const uint32_t wake_causes = esp_sleep_get_wakeup_causes();
    if (wake_causes & ~(1u << ESP_SLEEP_WAKEUP_UNDEFINED)) {
        ESP_LOGI(TAG, "休眠唤醒原因位图: 0x%02x", (unsigned)wake_causes);
    }
    if (wake_causes & (1u << ESP_SLEEP_WAKEUP_GPIO)) {
        key_guard_arm(&s_key_guard, esp_timer_get_time(), KEY_GUARD_MAX_US);
        ESP_LOGI(TAG, "按键唤醒:先屏蔽按键动作,直到那颗键松手");
    }

    if (bsp_display_init() != ESP_OK || !bsp_lvgl_init()) {
        ESP_LOGE(TAG, "display/LVGL init failed; check the panel and backlight wiring");
        return;
    }
    bsp_display_backlight(100);

    // 电量计是可选外设：失败时界面留空，不影响启动器与玩法。
    const esp_err_t battery = bsp_battery_init();
    if (battery != ESP_OK) {
        ESP_LOGW(TAG, "battery unavailable: %s", esp_err_to_name(battery));
    }

    const esp_err_t launcher = jerryicat_launcher_start();
    if (launcher != ESP_OK) {
        ESP_LOGE(TAG, "launcher start failed: %s", esp_err_to_name(launcher));
        return;
    }

    const esp_err_t input_err = input_dispatch_init();
    if (input_err != ESP_OK) {
        ESP_LOGE(TAG, "input dispatch init failed: %s", esp_err_to_name(input_err));
        return;
    }
    const esp_err_t button_err = bsp_button_init(on_key, NULL);
    if (button_err != ESP_OK) {
        ESP_LOGE(TAG, "button init failed: %s", esp_err_to_name(button_err));
        input_dispatch_deinit();
        return;
    }

    if (s_key_guard.armed) {
        // 若唤醒键在按键驱动启动前就已松手，组件不会产生 release 事件；
        // 初始化完成后按真实电压同步一次，已松手立即撤防。
        const bool released = bsp_button_read_mv() >= BSP_BTN_MV_RELEASED_MIN;
        key_guard_sync_released(&s_key_guard, released);
        if (!s_key_guard.armed) {
            ESP_LOGI(TAG, "唤醒键在按键驱动启动前已松手,提前撤防");
        }
    }

    s_input_ready = true;
    ESP_LOGI(TAG, "launcher ready");
}
