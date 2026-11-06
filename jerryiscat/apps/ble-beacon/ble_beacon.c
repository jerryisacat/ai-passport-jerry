#include "jerryiscat_app.h"
#include "jerryiscat_radio.h"
#include "jerryiscat_ui.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "lvgl.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include <string.h>

static const char *TAG = "ble_beacon";
static const char *DEVICE_NAME = "jerryiscat";

typedef enum {
    BS_STARTING = 0,
    BS_ADVERTISING,
    BS_FAILED,
    BS_OFF,
} bs_state_t;

static lv_obj_t *s_scr;
static lv_obj_t *s_status;
static lv_timer_t *s_timer;
static SemaphoreHandle_t s_host_stopped;
static TaskHandle_t s_host_task;
static volatile bs_state_t s_state;
static volatile int s_error;
static uint8_t s_addr_type;
static bool s_initialized;
static bool s_start_requested;
static bool s_stop_in_progress;
static bool s_host_done;

static int gap_event(struct ble_gap_event *event, void *arg);

static int advertise(void) {
    struct ble_hs_adv_fields fields = { 0 };
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (const uint8_t *)DEVICE_NAME;
    fields.name_len = (uint8_t)strlen(DEVICE_NAME);
    fields.name_is_complete = 1;

    int rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) return rc;

    struct ble_gap_adv_params params = { 0 };
    params.conn_mode = BLE_GAP_CONN_MODE_NON;
    params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(s_addr_type, NULL, BLE_HS_FOREVER, &params, gap_event, NULL);
    if (rc == 0) s_state = BS_ADVERTISING;
    return rc;
}

static int gap_event(struct ble_gap_event *event, void *arg) {
    (void)arg;
    if (event->type == BLE_GAP_EVENT_ADV_COMPLETE && s_start_requested) {
        int rc = advertise();
        if (rc != 0) {
            s_error = rc;
            s_state = BS_FAILED;
        }
    }
    return 0;
}

static void on_reset(int reason) {
    s_error = reason;
    s_state = BS_FAILED;
}

static void on_sync(void) {
    int rc = ble_hs_util_ensure_addr(0);
    if (rc == 0) rc = ble_hs_id_infer_auto(0, &s_addr_type);
    if (rc == 0 && s_start_requested) rc = advertise();
    if (rc != 0) {
        s_error = rc;
        s_state = BS_FAILED;
    }
}

static void host_task(void *arg) {
    (void)arg;
    nimble_port_run();
    xSemaphoreGive(s_host_stopped);
    for (;;) vTaskSuspend(NULL);
}

static esp_err_t ble_stop(void) {
    s_start_requested = false;
    s_state = BS_OFF;
    if (!s_initialized) return ESP_OK;

    if (s_host_task && !s_stop_in_progress) {
        (void)ble_gap_adv_stop();
        int rc = nimble_port_stop();
        if (rc != 0) {
            ESP_LOGE(TAG, "nimble_port_stop failed: %d", rc);
            return ESP_FAIL;
        }
        s_stop_in_progress = true;
    }

    if (s_host_task && !s_host_done) {
        if (!s_host_stopped ||
            xSemaphoreTake(s_host_stopped, pdMS_TO_TICKS(2000)) != pdTRUE) {
            ESP_LOGE(TAG, "waiting for NimBLE host timed out");
            return ESP_ERR_TIMEOUT;
        }
        s_host_done = true;
    }

    if (s_host_task) {
        vTaskDelete(s_host_task);
        s_host_task = NULL;
    }

    esp_err_t err = nimble_port_deinit();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nimble_port_deinit failed: %s", esp_err_to_name(err));
        return err;
    }
    s_initialized = false;
    s_stop_in_progress = false;
    s_host_done = false;
    if (s_host_stopped) {
        vSemaphoreDelete(s_host_stopped);
        s_host_stopped = NULL;
    }
    return ESP_OK;
}

static esp_err_t ble_start(void) {
    if (s_initialized) return ESP_ERR_INVALID_STATE;

    s_state = BS_STARTING;
    s_stop_in_progress = false;
    s_host_done = false;
    esp_err_t err = jerryiscat_radio_nvs_prepare();
    if (err != ESP_OK) goto fail;

    err = nimble_port_init();
    if (err != ESP_OK) goto fail;
    s_initialized = true;

    s_host_stopped = xSemaphoreCreateBinary();
    if (!s_host_stopped) {
        err = ESP_ERR_NO_MEM;
        goto fail;
    }

    ble_svc_gap_init();
    ble_svc_gatt_init();
    int rc = ble_svc_gap_device_name_set(DEVICE_NAME);
    if (rc != 0) {
        err = ESP_FAIL;
        goto fail;
    }

    ble_hs_cfg.reset_cb = on_reset;
    ble_hs_cfg.sync_cb = on_sync;
    s_start_requested = true;
    if (xTaskCreate(host_task, "nimble_host", NIMBLE_HS_STACK_SIZE, NULL,
                    configMAX_PRIORITIES - 4, &s_host_task) != pdPASS) {
        err = ESP_ERR_NO_MEM;
        goto fail;
    }
    return ESP_OK;

fail:
    s_start_requested = false;
    ESP_LOGE(TAG, "BLE start failed: %s", esp_err_to_name(err));
    (void)ble_stop();
    return err;
}

static void tick(lv_timer_t *timer) {
    (void)timer;
    switch (s_state) {
        case BS_STARTING:
            lv_label_set_text(s_status, "Starting NimBLE...");
            break;
        case BS_ADVERTISING:
            lv_label_set_text(s_status, "Advertising\n\nName: jerryiscat\n\nUse a BLE scanner\non your phone.");
            break;
        case BS_FAILED:
            lv_label_set_text_fmt(s_status, "BLE failed: %d", s_error);
            s_state = BS_OFF;
            break;
        default:
            break;
    }
}

static esp_err_t ble_enter(void) {
    s_scr = lv_obj_create(NULL);
    jerryiscat_ui_screen_init(s_scr);
    jerryiscat_ui_brand(s_scr, "BLE");
    jerryiscat_ui_battery(s_scr);

    lv_obj_t *title = lv_label_create(s_scr);
    lv_label_set_text(title, "BLE BEACON");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(JC_COLOR_TEXT), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 20, 48);

    s_status = lv_label_create(s_scr);
    lv_obj_set_width(s_status, 200);
    lv_label_set_long_mode(s_status, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(s_status, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_status, lv_color_hex(JC_COLOR_MUTED), 0);
    lv_obj_align(s_status, LV_ALIGN_TOP_LEFT, 20, 96);
    lv_label_set_text(s_status, "Starting NimBLE...");

    jerryiscat_ui_hint(s_scr, "OK LONG  BACK");
    lv_screen_load(s_scr);

    s_state = BS_STARTING;
    s_timer = lv_timer_create(tick, 100, NULL);
    return ESP_OK;
}

static void ble_exit(void) {
    if (s_timer) {
        lv_timer_delete(s_timer);
        s_timer = NULL;
    }
    if (s_scr) {
        lv_obj_delete(s_scr);
        s_scr = NULL;
    }
    s_status = NULL;
}

const jerryiscat_app_t jerryiscat_ble_beacon_app = {
    .id = "ble-beacon",
    .title = "BLE Beacon",
    .kind = JERRYISCAT_APP_TOOL,
    .enter = ble_enter,
    .exit = ble_exit,
    .key = NULL,
    .start = ble_start,
    .stop = ble_stop,
};
