#include "jerryicat_internal.h"

// 每个玩法/设备工具在自己的 .c 中定义这个描述符，由这里显式登记。
extern const jerryicat_app_t jerryicat_plus_one_app;

static const jerryicat_app_t *const s_apps[] = {
    &jerryicat_plus_one_app,
};

size_t jerryicat_registry_count(void) {
    return sizeof(s_apps) / sizeof(s_apps[0]);
}

const jerryicat_app_t *jerryicat_registry_at(size_t index) {
    if (index >= jerryicat_registry_count()) return NULL;
    return s_apps[index];
}
