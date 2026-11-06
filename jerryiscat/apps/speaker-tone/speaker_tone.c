#include "jerryiscat_app.h"
#include "jerryiscat_ui.h"

#include "bsp_audio.h"
#include "bsp_display.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"

#include <stdint.h>

static const char *TAG = "speaker_tone";
#define SAMPLE_RATE 16000
#define CHUNK_SAMPLES 160
#define TONE_HZ 500
#define TONE_SECONDS 2

static lv_obj_t *s_scr;
static lv_obj_t *s_status;
static volatile bool s_stop;
static volatile bool s_done = true;

static void set_status(const char *text) {
    if (!bsp_lvgl_lock(200)) return;
    if (s_status) lv_label_set_text(s_status, text);
    bsp_lvgl_unlock();
}

static void tone_task(void *arg) {
    (void)arg;
    int16_t buf[CHUNK_SAMPLES];
    uint32_t acc = 0;
    int level = 6000;
    int total = SAMPLE_RATE * TONE_SECONDS;

    while (!s_stop && total > 0) {
        int n = total < CHUNK_SAMPLES ? total : CHUNK_SAMPLES;
        for (int i = 0; i < n; i++) {
            acc += TONE_HZ;
            if (acc >= SAMPLE_RATE) {
                acc -= SAMPLE_RATE;
                level = -level;
            }
            buf[i] = (int16_t)level;
        }
        if (bsp_audio_write(buf, (size_t)n * sizeof(int16_t)) != ESP_OK) {
            set_status("Playback failed");
            break;
        }
        total -= n;
    }
    if (!s_stop && total == 0) set_status("Done");
    s_done = true;
    vTaskDelete(NULL);
}

static esp_err_t tone_start(void) {
    esp_err_t err = bsp_audio_init();
    if (err != ESP_OK) return err;
    err = bsp_audio_wake();
    if (err != ESP_OK) return err;
    err = bsp_audio_set_format(SAMPLE_RATE, 16, 1);
    if (err != ESP_OK) return err;
    bsp_audio_set_volume(40);

    s_stop = false;
    s_done = false;
    if (xTaskCreate(tone_task, "speaker_tone", 4096, NULL, 4, NULL) != pdPASS) {
        s_done = true;
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static esp_err_t tone_stop(void) {
    s_stop = true;
    for (int i = 0; i < 200 && !s_done; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!s_done) {
        ESP_LOGE(TAG, "tone task did not stop in time");
        return ESP_ERR_TIMEOUT;
    }
    return bsp_audio_sleep();
}

static esp_err_t tone_enter(void) {
    s_scr = lv_obj_create(NULL);
    jerryiscat_ui_screen_init(s_scr);
    jerryiscat_ui_brand(s_scr, "SPEAKER");
    jerryiscat_ui_battery(s_scr);

    lv_obj_t *title = lv_label_create(s_scr);
    lv_label_set_text(title, "SPEAKER TONE");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(JC_COLOR_TEXT), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 20, 48);

    s_status = lv_label_create(s_scr);
    lv_obj_set_width(s_status, 200);
    lv_label_set_long_mode(s_status, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(s_status, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_status, lv_color_hex(JC_COLOR_MUTED), 0);
    lv_obj_align(s_status, LV_ALIGN_TOP_LEFT, 20, 110);
    lv_label_set_text(s_status, "Playing 500 Hz for 2s...");

    jerryiscat_ui_hint(s_scr, "OK LONG  BACK");
    lv_screen_load(s_scr);
    return ESP_OK;
}

static void tone_exit(void) {
    if (s_scr) {
        lv_obj_delete(s_scr);
        s_scr = NULL;
    }
    s_status = NULL;
}

const jerryiscat_app_t jerryiscat_speaker_tone_app = {
    .id = "speaker-tone",
    .title = "Speaker Tone",
    .kind = JERRYISCAT_APP_TOOL,
    .enter = tone_enter,
    .exit = tone_exit,
    .key = NULL,
    .start = tone_start,
    .stop = tone_stop,
};
