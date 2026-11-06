#include "jerryiscat_app.h"
#include "jerryiscat_ui.h"

#include "bsp_audio.h"
#include "bsp_display.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"

#include <stdint.h>

static const char *TAG = "mic_level";
#define SAMPLE_RATE 16000
#define CHUNK_SAMPLES 160

static lv_obj_t *s_scr;
static lv_obj_t *s_bar;
static lv_obj_t *s_status;
static volatile bool s_stop;
static volatile bool s_done = true;

static void set_status(const char *text) {
    if (!bsp_lvgl_lock(200)) return;
    if (s_status) lv_label_set_text(s_status, text);
    bsp_lvgl_unlock();
}

static void mic_task(void *arg) {
    (void)arg;
    int16_t buf[CHUNK_SAMPLES];
    int peak = 0;
    int throttle = 0;

    while (!s_stop) {
        if (bsp_audio_read(buf, sizeof(buf)) != ESP_OK) {
            set_status("Mic read failed");
            break;
        }
        int local = 0;
        for (size_t i = 0; i < CHUNK_SAMPLES; i++) {
            int v = buf[i];
            if (v < 0) v = -v;
            if (v > local) local = v;
        }
        peak = (peak * 3 + local) / 4;
        if (++throttle >= 3) {
            throttle = 0;
            if (bsp_lvgl_lock(100)) {
                if (s_bar) lv_bar_set_value(s_bar, (peak * 100) / 32768, LV_ANIM_OFF);
                bsp_lvgl_unlock();
            }
        }
    }
    s_done = true;
    vTaskDelete(NULL);
}

static esp_err_t mic_start(void) {
    esp_err_t err = bsp_audio_init();
    if (err != ESP_OK) return err;
    err = bsp_audio_wake();
    if (err != ESP_OK) return err;
    err = bsp_audio_set_format(SAMPLE_RATE, 16, 1);
    if (err != ESP_OK) return err;
    bsp_audio_set_volume(0);

    s_stop = false;
    s_done = false;
    if (xTaskCreate(mic_task, "mic_level", 4096, NULL, 4, NULL) != pdPASS) {
        s_done = true;
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static esp_err_t mic_stop(void) {
    s_stop = true;
    for (int i = 0; i < 200 && !s_done; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!s_done) {
        ESP_LOGE(TAG, "mic task did not stop in time");
        return ESP_ERR_TIMEOUT;
    }
    return bsp_audio_sleep();
}

static esp_err_t mic_enter(void) {
    s_scr = lv_obj_create(NULL);
    jerryiscat_ui_screen_init(s_scr);
    jerryiscat_ui_brand(s_scr, "MIC");
    jerryiscat_ui_battery(s_scr);

    lv_obj_t *title = lv_label_create(s_scr);
    lv_label_set_text(title, "MIC LEVEL");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(JC_COLOR_TEXT), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 20, 48);

    s_bar = lv_bar_create(s_scr);
    lv_obj_set_size(s_bar, 180, 14);
    lv_obj_align(s_bar, LV_ALIGN_TOP_MID, 0, 140);
    lv_bar_set_range(s_bar, 0, 100);
    lv_bar_set_value(s_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(s_bar, lv_color_hex(JC_COLOR_PANEL), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_bar, lv_color_hex(JC_COLOR_ACCENT), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(s_bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(s_bar, 7, LV_PART_MAIN);
    lv_obj_set_style_radius(s_bar, 7, LV_PART_INDICATOR);

    s_status = lv_label_create(s_scr);
    lv_obj_set_style_text_font(s_status, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_status, lv_color_hex(JC_COLOR_MUTED), 0);
    lv_obj_align(s_status, LV_ALIGN_TOP_MID, 0, 180);
    lv_label_set_text(s_status, "Listening...");

    jerryiscat_ui_hint(s_scr, "OK LONG  BACK");
    lv_screen_load(s_scr);
    return ESP_OK;
}

static void mic_exit(void) {
    if (s_scr) {
        lv_obj_delete(s_scr);
        s_scr = NULL;
    }
    s_bar = NULL;
    s_status = NULL;
}

const jerryiscat_app_t jerryiscat_mic_level_app = {
    .id = "mic-level",
    .title = "Mic Level",
    .kind = JERRYISCAT_APP_TOOL,
    .enter = mic_enter,
    .exit = mic_exit,
    .key = NULL,
    .start = mic_start,
    .stop = mic_stop,
};
