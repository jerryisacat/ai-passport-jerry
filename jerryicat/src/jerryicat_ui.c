#include "jerryicat_ui.h"

#include "bsp_battery.h"

void jerryicat_ui_screen_init(lv_obj_t *scr) {
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(scr, lv_color_hex(JC_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr, 0, 0);
    lv_obj_set_style_pad_all(scr, 0, 0);
}

lv_obj_t *jerryicat_ui_brand(lv_obj_t *parent, const char *text) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(JC_COLOR_ACCENT), 0);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 20, 18);
    return label;
}

lv_obj_t *jerryicat_ui_battery(lv_obj_t *parent) {
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(JC_COLOR_MUTED), 0);
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -20, 18);
    jerryicat_ui_battery_refresh(label);
    return label;
}

void jerryicat_ui_battery_refresh(lv_obj_t *label) {
    if (!label) return;
    const int soc = bsp_battery_soc();
    if (soc < 0) {
        lv_label_set_text(label, "");
        return;
    }
    lv_label_set_text_fmt(label, "%d%%", soc);
}

lv_obj_t *jerryicat_ui_hint(lv_obj_t *parent, const char *text) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(JC_COLOR_MUTED), 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -22);
    return label;
}
