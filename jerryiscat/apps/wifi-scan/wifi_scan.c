#include "jerryiscat_app.h"
#include "jerryiscat_radio.h"
#include "jerryiscat_ui.h"

#include "bsp_display.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "lvgl.h"

#include <stdio.h>
#include <string.h>

static const char *TAG = "wifi_scan";
#define MAX_APS 5
#define SCAN_TIMEOUT_MS 8000

typedef enum {
    WS_STARTING = 0,
    WS_SCANNING,
    WS_READY,
    WS_FAILED,
    WS_OFF,
} ws_state_t;

static lv_obj_t *s_scr;
static lv_obj_t *s_status;
static lv_obj_t *s_results;
static lv_timer_t *s_timer;
static esp_netif_t *s_netif;
static esp_event_handler_instance_t s_handler;
static bool s_wifi_inited;
static bool s_wifi_started;
static bool s_handler_registered;
static volatile ws_state_t s_state;
static volatile esp_err_t s_error;
static uint32_t s_scan_started_ms;

static void scan_done(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg;
    (void)base;
    (void)id;
    (void)data;
    if (s_state == WS_SCANNING) s_state = WS_READY;
}

static void wifi_stack_stop(void) {
    s_state = WS_OFF;
    if (s_wifi_started) {
        (void)esp_wifi_scan_stop();
        (void)esp_wifi_stop();
        s_wifi_started = false;
    }
    if (s_handler_registered) {
        (void)esp_event_handler_instance_unregister(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, s_handler);
        s_handler_registered = false;
    }
    if (s_wifi_inited) {
        (void)esp_wifi_deinit();
        s_wifi_inited = false;
    }
    if (s_netif) {
        esp_netif_destroy_default_wifi(s_netif);
        s_netif = NULL;
    }
}

static void show_results(void) {
    uint16_t total = 0;
    uint16_t count = MAX_APS;
    static wifi_ap_record_t records[MAX_APS];
    static char text[320];
    size_t used = 0;

    esp_err_t err = esp_wifi_scan_get_ap_num(&total);
    if (err == ESP_OK) err = esp_wifi_scan_get_ap_records(&count, records);
    if (err != ESP_OK) {
        s_error = err;
        s_state = WS_FAILED;
        return;
    }

    text[0] = '\0';
    for (uint16_t i = 0; i < count && used < sizeof(text); i++) {
        const char *ssid = (const char *)records[i].ssid;
        if (ssid[0] == '\0') ssid = "(hidden)";
        int written = snprintf(text + used, sizeof(text) - used, "%d  %.18s  ch%u\n",
                               records[i].rssi, ssid, records[i].primary);
        if (written < 0 || (size_t)written >= sizeof(text) - used) break;
        used += (size_t)written;
    }
    if (count == 0) snprintf(text, sizeof(text), "No access points found");

    lv_label_set_text_fmt(s_status, "%u APs found", total);
    lv_label_set_text(s_results, text);
    s_state = WS_OFF;
}

static void tick(lv_timer_t *timer) {
    (void)timer;
    switch (s_state) {
        case WS_STARTING:
            lv_label_set_text(s_status, "Starting Wi-Fi...");
            break;
        case WS_SCANNING:
            if (lv_tick_elaps(s_scan_started_ms) > SCAN_TIMEOUT_MS) {
                s_error = ESP_ERR_TIMEOUT;
                s_state = WS_FAILED;
            } else {
                lv_label_set_text(s_status, "Scanning 2.4 GHz...");
            }
            break;
        case WS_READY:
            show_results();
            break;
        case WS_FAILED:
            lv_label_set_text_fmt(s_status, "Wi-Fi failed: %s", esp_err_to_name(s_error));
            s_state = WS_OFF;
            break;
        default:
            break;
    }
}

static esp_err_t wifi_start(void) {
    esp_err_t err = jerryiscat_radio_nvs_prepare();
    if (err != ESP_OK) goto fail;
    err = jerryiscat_radio_netif_prepare();
    if (err != ESP_OK) goto fail;

    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    s_netif = esp_netif_new(&netif_cfg);
    if (!s_netif) {
        err = ESP_ERR_NO_MEM;
        goto fail;
    }
    err = esp_netif_attach_wifi_station(s_netif);
    if (err != ESP_OK) goto fail;
    err = esp_wifi_set_default_wifi_sta_handlers();
    if (err != ESP_OK) goto fail;

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) goto fail;
    s_wifi_inited = true;

    err = esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE,
                                              scan_done, NULL, &s_handler);
    if (err != ESP_OK) goto fail;
    s_handler_registered = true;

    err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (err != ESP_OK) goto fail;
    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) goto fail;
    err = esp_wifi_start();
    if (err != ESP_OK) goto fail;
    s_wifi_started = true;

    s_scan_started_ms = lv_tick_get();
    err = esp_wifi_scan_start(NULL, false);
    if (err != ESP_OK) goto fail;
    s_state = WS_SCANNING;
    return ESP_OK;

fail:
    ESP_LOGE(TAG, "Wi-Fi start failed: %s", esp_err_to_name(err));
    wifi_stack_stop();
    s_error = err;
    s_state = WS_FAILED;
    return err;
}

static esp_err_t wifi_enter(void) {
    s_scr = lv_obj_create(NULL);
    jerryiscat_ui_screen_init(s_scr);
    jerryiscat_ui_brand(s_scr, "WI-FI");
    jerryiscat_ui_battery(s_scr);

    lv_obj_t *title = lv_label_create(s_scr);
    lv_label_set_text(title, "WI-FI SCAN");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(JC_COLOR_TEXT), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 20, 48);

    s_status = lv_label_create(s_scr);
    lv_obj_set_width(s_status, 200);
    lv_obj_set_style_text_font(s_status, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_status, lv_color_hex(JC_COLOR_MUTED), 0);
    lv_obj_align(s_status, LV_ALIGN_TOP_LEFT, 20, 88);
    lv_label_set_text(s_status, "Starting Wi-Fi...");

    s_results = lv_label_create(s_scr);
    lv_obj_set_width(s_results, 200);
    lv_label_set_long_mode(s_results, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(s_results, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_results, lv_color_hex(JC_COLOR_TEXT), 0);
    lv_obj_align(s_results, LV_ALIGN_TOP_LEFT, 20, 116);
    lv_label_set_text(s_results, "");

    jerryiscat_ui_hint(s_scr, "OK LONG  BACK");
    lv_screen_load(s_scr);

    s_state = WS_STARTING;
    s_timer = lv_timer_create(tick, 100, NULL);
    return ESP_OK;
}

static void wifi_exit(void) {
    if (s_timer) {
        lv_timer_delete(s_timer);
        s_timer = NULL;
    }
    if (s_scr) {
        lv_obj_delete(s_scr);
        s_scr = NULL;
    }
    s_status = NULL;
    s_results = NULL;
}

static esp_err_t wifi_stop(void) {
    wifi_stack_stop();
    return ESP_OK;
}

const jerryiscat_app_t jerryiscat_wifi_scan_app = {
    .id = "wifi-scan",
    .title = "Wi-Fi Scan",
    .kind = JERRYISCAT_APP_TOOL,
    .enter = wifi_enter,
    .exit = wifi_exit,
    .key = NULL,
    .start = wifi_start,
    .stop = wifi_stop,
};
