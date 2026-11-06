#pragma once

#include "bsp_button.h"
#include "esp_err.h"

// 设备端应用类别。启动器只按类别分组展示；两者生命周期完全相同。
typedef enum {
    JERRYICAT_APP_PLAY = 0,
    JERRYICAT_APP_TOOL,
} jerryicat_app_kind_t;

// 设备端应用描述符。id 与目录名一致，title 是启动器显示名。
//
// 调用上下文（由启动器保证）：
// - enter/exit 在已持有 bsp_lvgl_lock() 时调用，可直接操作 lv_* 对象。
// - key 不持锁，由固件输入任务经 jerryicat_handle_key() 调用；需要更新 UI 时
//   自行调用 bsp_lvgl_lock()/bsp_lvgl_unlock()，并且不得阻塞。启动器保留
//   OK 长按作为返回手势，因此该事件不会转给应用。
// - start/stop 不持锁，用于启动/停止工作任务和 I/O；可以为 NULL。
typedef struct jerryicat_app {
    const char *id;
    const char *title;
    jerryicat_app_kind_t kind;
    esp_err_t (*enter)(void);
    void (*exit)(void);
    void (*key)(bsp_btn_t btn, bsp_btn_ev_t ev);
    esp_err_t (*start)(void);
    esp_err_t (*stop)(void);
} jerryicat_app_t;
