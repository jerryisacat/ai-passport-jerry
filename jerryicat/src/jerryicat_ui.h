#pragma once

#include "lvgl.h"

// 共享视觉常量。启动器与各应用复用同一套暗色调，但每个应用自行决定布局。
#define JC_COLOR_BG 0x0B0F14
#define JC_COLOR_PANEL 0x171D25
#define JC_COLOR_LINE 0x2A3441
#define JC_COLOR_TEXT 0xF2F5F7
#define JC_COLOR_MUTED 0x8794A3
#define JC_COLOR_ACCENT 0xF5B84B
#define JC_COLOR_ACCENT_INK 0x1A1305

// 页面根对象的基础样式：无滚动、铺满背景、无边框内边距。
void jerryicat_ui_screen_init(lv_obj_t *scr);

// 左上角品牌字与右上角电量。电量读取失败时留空。
lv_obj_t *jerryicat_ui_brand(lv_obj_t *parent, const char *text);
lv_obj_t *jerryicat_ui_battery(lv_obj_t *parent);
void jerryicat_ui_battery_refresh(lv_obj_t *label);

// 底部操作提示。
lv_obj_t *jerryicat_ui_hint(lv_obj_t *parent, const char *text);
