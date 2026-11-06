#pragma once

#include "bsp_button.h"
#include "esp_err.h"

// 启动 fork 启动器：创建默认界面并载入。
// 必须在 bsp_display_init() 与 bsp_lvgl_init() 成功后调用。
esp_err_t jerryicat_launcher_start(void);

// 把一次按键事件交给启动器或当前应用。
// 由固件的输入任务调用（不是按键回调任务），不得阻塞；启动器内部按需加 LVGL 锁。
void jerryicat_handle_key(bsp_btn_t btn, bsp_btn_ev_t ev);
