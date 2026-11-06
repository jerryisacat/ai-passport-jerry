#include "jerryiscat_radio.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

static const char *TAG = "jerryiscat_radio";

static bool s_nvs_ready;
static bool s_netif_ready;
static bool s_event_ready;

esp_err_t jerryiscat_radio_nvs_prepare(void) {
    if (s_nvs_ready) return ESP_OK;

    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s; not erasing user data", esp_err_to_name(err));
        return err;
    }
    s_nvs_ready = true;
    return ESP_OK;
}

esp_err_t jerryiscat_radio_netif_prepare(void) {
    if (!s_netif_ready) {
        esp_err_t err = esp_netif_init();
        if (err != ESP_OK) return err;
        s_netif_ready = true;
    }
    if (!s_event_ready) {
        esp_err_t err = esp_event_loop_create_default();
        if (err != ESP_OK) return err;
        s_event_ready = true;
    }
    return ESP_OK;
}
