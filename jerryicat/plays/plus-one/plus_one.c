#include "plus_one_logic.h"

#include "bsp_display.h"
#include "jerryicat_app.h"
#include "jerryicat_ui.h"
#include "lvgl.h"

// 计数随应用实例保留：回到启动器再进来仍是同一个数，重启后从 0 开始。
static uint32_t s_count;
static lv_obj_t *s_scr;
static lv_obj_t *s_value;

static void refresh_value(void) {
    if (!s_value) return;
    lv_label_set_text_fmt(s_value, "%lu", (unsigned long)s_count);
}

static esp_err_t plus_one_enter(void) {
    s_scr = lv_obj_create(NULL);
    if (!s_scr) return ESP_ERR_NO_MEM;
    jerryicat_ui_screen_init(s_scr);
    jerryicat_ui_brand(s_scr, "+1");
    jerryicat_ui_battery(s_scr);

    s_value = lv_label_create(s_scr);
    lv_obj_set_style_text_font(s_value, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(s_value, lv_color_hex(JC_COLOR_ACCENT), 0);
    lv_obj_align(s_value, LV_ALIGN_CENTER, 0, -18);
    refresh_value();

    lv_obj_t *caption = lv_label_create(s_scr);
    lv_label_set_text(caption, "PRESSES THIS SESSION");
    lv_obj_set_style_text_font(caption, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(caption, lv_color_hex(JC_COLOR_MUTED), 0);
    lv_obj_align(caption, LV_ALIGN_CENTER, 0, 34);

    jerryicat_ui_hint(s_scr, "PRESS ANY KEY +1   HOLD OK BACK");
    lv_screen_load(s_scr);
    return ESP_OK;
}

static void plus_one_exit(void) {
    if (s_scr) {
        lv_obj_delete(s_scr);
        s_scr = NULL;
    }
    s_value = NULL;
}

static void plus_one_key(bsp_btn_t btn, bsp_btn_ev_t ev) {
    (void)btn;
    // 按下即计数：每次物理按压恰好一个 PRESS，连按、双击和长按都不会漏计。
    // 长按 OK 返回菜单时，启动那次手势的按压同样算一次。
    if (ev != BSP_BTN_PRESS) return;

    s_count = plus_one_increment(s_count);
    if (!bsp_lvgl_lock(100)) return;
    refresh_value();
    bsp_lvgl_unlock();
}

const jerryicat_app_t jerryicat_plus_one_app = {
    .id = "plus-one",
    .title = "+1",
    .kind = JERRYICAT_APP_PLAY,
    .enter = plus_one_enter,
    .exit = plus_one_exit,
    .key = plus_one_key,
    .start = NULL,
    .stop = NULL,
};
