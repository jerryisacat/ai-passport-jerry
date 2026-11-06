#include "jerryiscat_internal.h"

// 每个设备端工具在自己的 .c 中定义描述符，由这里显式登记。
extern const jerryiscat_app_t jerryiscat_wifi_scan_app;
extern const jerryiscat_app_t jerryiscat_ble_beacon_app;
extern const jerryiscat_app_t jerryiscat_mic_level_app;
extern const jerryiscat_app_t jerryiscat_speaker_tone_app;

static const jerryiscat_app_t *const s_apps[] = {
    &jerryiscat_wifi_scan_app,
    &jerryiscat_ble_beacon_app,
    &jerryiscat_mic_level_app,
    &jerryiscat_speaker_tone_app,
};

size_t jerryiscat_registry_count(void) {
    return sizeof(s_apps) / sizeof(s_apps[0]);
}

const jerryiscat_app_t *jerryiscat_registry_at(size_t index) {
    if (index >= jerryiscat_registry_count()) return NULL;
    return s_apps[index];
}
