#include "jerryicat.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bsp_display.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "jerryicat_app.h"
#include "jerryicat_internal.h"
#include "jerryicat_nav.h"
#include "jerryicat_ui.h"
#include "lvgl.h"

static const char *TAG = "jerryicat";

#define MAX_APPS 12
#define UI_LOCK_MS 300
#define BACK_GUARD_US (300 * 1000)

static lv_obj_t *s_menu_scr;
static lv_obj_t *s_cards[MAX_APPS];
static lv_obj_t *s_titles[MAX_APPS];
static lv_obj_t *s_kinds[MAX_APPS];
static size_t s_app_count;
static int s_selected;
static const jerryicat_app_t *s_active;
static bool s_ui_ready;
static int64_t s_ignore_click_until;

static void apply_selection(int selected) {
    for (size_t i = 0; i < s_app_count; i++) {
        const bool on = ((int)i == selected);
        if (s_cards[i]) {
            lv_obj_set_style_bg_color(
                s_cards[i], lv_color_hex(on ? JC_COLOR_ACCENT : JC_COLOR_PANEL), 0);
            lv_obj_set_style_border_color(
                s_cards[i], lv_color_hex(on ? JC_COLOR_ACCENT : JC_COLOR_LINE), 0);
        }
        if (s_titles[i]) {
            lv_obj_set_style_text_color(
                s_titles[i], lv_color_hex(on ? JC_COLOR_ACCENT_INK : JC_COLOR_TEXT), 0);
        }
        if (s_kinds[i]) {
            lv_obj_set_style_text_color(
                s_kinds[i], lv_color_hex(on ? JC_COLOR_ACCENT_INK : JC_COLOR_MUTED), 0);
        }
    }
}

static void build_menu(const char *hint_text) {
    for (size_t i = 0; i < MAX_APPS; i++) {
        s_cards[i] = NULL;
        s_titles[i] = NULL;
        s_kinds[i] = NULL;
    }

    s_menu_scr = lv_obj_create(NULL);
    jerryicat_ui_screen_init(s_menu_scr);
    jerryicat_ui_brand(s_menu_scr, "JERRYICAT");
    jerryicat_ui_battery(s_menu_scr);

    lv_obj_t *heading = lv_label_create(s_menu_scr);
    lv_label_set_text(heading, "PLAYS");
    lv_obj_set_style_text_font(heading, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(heading, lv_color_hex(JC_COLOR_TEXT), 0);
    lv_obj_align(heading, LV_ALIGN_TOP_LEFT, 20, 48);

    lv_obj_t *list = lv_obj_create(s_menu_scr);
    lv_obj_set_size(list, 208, 180);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 86);
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_pad_row(list, 10, 0);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

    s_app_count = jerryicat_registry_count();
    if (s_app_count > MAX_APPS) s_app_count = MAX_APPS;
    if (s_app_count == 0) {
        s_selected = 0;
        lv_obj_t *empty = lv_label_create(s_menu_scr);
        lv_label_set_text(empty, "NO APPS INSTALLED");
        lv_obj_set_style_text_font(empty, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(empty, lv_color_hex(JC_COLOR_MUTED), 0);
        lv_obj_align(empty, LV_ALIGN_CENTER, 0, 0);
    } else {
        if (s_selected >= (int)s_app_count) s_selected = (int)s_app_count - 1;

        for (size_t i = 0; i < s_app_count; i++) {
            const jerryicat_app_t *app = jerryicat_registry_at(i);

            lv_obj_t *card = lv_obj_create(list);
            lv_obj_set_size(card, 208, 52);
            lv_obj_set_style_radius(card, 12, 0);
            lv_obj_set_style_border_width(card, 1, 0);
            lv_obj_set_style_border_color(card, lv_color_hex(JC_COLOR_LINE), 0);
            lv_obj_set_style_bg_color(card, lv_color_hex(JC_COLOR_PANEL), 0);
            lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
            lv_obj_set_style_pad_all(card, 0, 0);
            lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);

            lv_obj_t *title = lv_label_create(card);
            lv_label_set_text(title, app->title ? app->title : app->id);
            lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
            lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);

            lv_obj_t *kind = lv_label_create(card);
            lv_label_set_text(kind, app->kind == JERRYICAT_APP_TOOL ? "TOOL" : "PLAY");
            lv_obj_set_style_text_font(kind, &lv_font_montserrat_14, 0);
            lv_obj_align(kind, LV_ALIGN_RIGHT_MID, -16, 0);

            s_cards[i] = card;
            s_titles[i] = title;
            s_kinds[i] = kind;
        }
        apply_selection(s_selected);
        lv_obj_scroll_to_view(s_cards[s_selected], LV_ANIM_OFF);
    }

    jerryicat_ui_hint(
        s_menu_scr, hint_text ? hint_text : "UP/DOWN  SELECT     OK  OPEN");
    lv_screen_load(s_menu_scr);
}

static void move_selection(int delta) {
    if (s_app_count < 2) return;
    if (!bsp_lvgl_lock(UI_LOCK_MS)) return;

    const int next = jerryicat_nav_next(s_selected, delta, (int)s_app_count);
    if (next >= 0 && next != s_selected) {
        s_selected = next;
        apply_selection(s_selected);
        if (s_cards[s_selected]) lv_obj_scroll_to_view(s_cards[s_selected], LV_ANIM_ON);
    }
    bsp_lvgl_unlock();
}

static void leave_app(const char *hint_text) {
    const jerryicat_app_t *app = s_active;
    if (!app) return;

    if (app->stop) {
        const esp_err_t err = app->stop();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s stop failed: %s", app->id, esp_err_to_name(err));
        }
    }

    if (!bsp_lvgl_lock(1000)) return;
    s_active = NULL;
    if (app->exit) app->exit();
    build_menu(hint_text);
    bsp_lvgl_unlock();

    // 长按松手后可能紧跟着一个 click；短暂挡住，避免刚回菜单又被同一个手势打开。
    s_ignore_click_until = esp_timer_get_time() + BACK_GUARD_US;
}

static void enter_app(void) {
    if (s_app_count == 0) return;
    const jerryicat_app_t *app = jerryicat_registry_at((size_t)s_selected);
    if (!app) return;
    if (!bsp_lvgl_lock(UI_LOCK_MS)) return;

    lv_obj_t *menu = s_menu_scr;
    s_menu_scr = NULL;
    if (menu) lv_obj_delete(menu);

    const esp_err_t enter_err = app->enter ? app->enter() : ESP_OK;
    if (enter_err != ESP_OK) {
        ESP_LOGE(TAG, "%s enter failed: %s", app->id, esp_err_to_name(enter_err));
        build_menu("APP FAILED TO START");
        bsp_lvgl_unlock();
        return;
    }
    s_active = app;
    bsp_lvgl_unlock();

    if (app->start) {
        const esp_err_t start_err = app->start();
        if (start_err != ESP_OK) {
            ESP_LOGE(TAG, "%s start failed: %s", app->id, esp_err_to_name(start_err));
            leave_app("APP FAILED TO START");
        }
    }
}

void jerryicat_handle_key(bsp_btn_t btn, bsp_btn_ev_t ev) {
    if (!s_ui_ready) return;

    if (s_active) {
        // OK 长按是启动器保留的返回手势；其余事件交给当前应用。
        if (btn == BSP_BTN_OK && ev == BSP_BTN_LONG) {
            leave_app(NULL);
            return;
        }
        if (s_active->key) s_active->key(btn, ev);
        return;
    }

    if (ev != BSP_BTN_CLICK) return;
    if (esp_timer_get_time() < s_ignore_click_until) return;

    switch (btn) {
        case BSP_BTN_UP:
            move_selection(-1);
            break;
        case BSP_BTN_DOWN:
            move_selection(1);
            break;
        case BSP_BTN_OK:
            enter_app();
            break;
    }
}

esp_err_t jerryicat_launcher_start(void) {
    if (!bsp_lvgl_lock(1000)) return ESP_ERR_TIMEOUT;
    build_menu(NULL);
    bsp_lvgl_unlock();

    s_ui_ready = true;
    ESP_LOGI(TAG, "launcher ready with %u app(s)", (unsigned)s_app_count);
    return ESP_OK;
}
