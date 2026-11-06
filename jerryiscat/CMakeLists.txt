idf_component_register(
    SRCS "src/jerryiscat_launcher.c"
         "src/jerryiscat_nav.c"
         "src/jerryiscat_registry.c"
         "src/jerryiscat_ui.c"
         "src/jerryiscat_radio.c"
         "apps/wifi-scan/wifi_scan.c"
         "apps/ble-beacon/ble_beacon.c"
         "apps/mic-level/mic_level.c"
         "apps/speaker-tone/speaker_tone.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    REQUIRES bsp lvgl
    PRIV_REQUIRES esp_timer esp_wifi esp_netif esp_event nvs_flash bt
)
