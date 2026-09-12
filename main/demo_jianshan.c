#include "demo.h"
#include "bsp_battery.h"
#include "esp_random.h"
#include "jianshan_cards.h"
#include "lvgl.h"
#include "ui_pixel.h"

LV_FONT_DECLARE(jianshan_font_16);
LV_FONT_DECLARE(jianshan_font_22);

static lv_obj_t *s_scr;
static lv_obj_t *s_title;
static lv_obj_t *s_body;
static lv_obj_t *s_source;
static lv_obj_t *s_position;
static lv_obj_t *s_battery;
static jianshan_state_t s_state;

static void refresh_card(void)
{
    size_t index = jianshan_state_current(&s_state);
    const jianshan_card_t *card = &jianshan_cards[index];

    lv_label_set_text(s_title, card->title);
    lv_label_set_text(s_body, card->body);
    lv_label_set_text(s_source, card->source);
    lv_label_set_text_fmt(s_position, "%u/%u",
                          (unsigned)(s_state.history_pos + 1),
                          (unsigned)s_state.history_len);

    int soc = bsp_battery_soc();
    if (soc < 0) lv_label_set_text(s_battery, "--%");
    else         lv_label_set_text_fmt(s_battery, "%d%%", soc);
}

void demo_jianshan_enter(void)
{
    s_scr = ui_pixel_screen_create("JIANSHAN");
    lv_obj_t *panel = ui_pixel_panel_create(s_scr, 10, 52, 220, 225, UI_PAPER);

    s_battery = lv_label_create(s_scr);
    lv_obj_set_style_text_font(s_battery, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_battery, lv_color_hex(UI_INK), 0);
    lv_obj_set_pos(s_battery, 183, 34);

    s_title = lv_label_create(panel);
    lv_obj_set_style_text_font(s_title, &jianshan_font_16, 0);
    lv_obj_set_style_text_color(s_title, lv_color_hex(UI_RED), 0);
    lv_obj_set_width(s_title, 190);
    lv_obj_align(s_title, LV_ALIGN_TOP_LEFT, 0, 0);

    s_body = lv_label_create(panel);
    lv_obj_set_style_text_font(s_body, &jianshan_font_22, 0);
    lv_obj_set_style_text_color(s_body, lv_color_hex(UI_INK), 0);
    lv_obj_set_style_text_line_space(s_body, 7, 0);
    lv_obj_set_width(s_body, 190);
    lv_label_set_long_mode(s_body, LV_LABEL_LONG_WRAP);
    lv_obj_align(s_body, LV_ALIGN_LEFT_MID, 0, -2);

    s_source = lv_label_create(panel);
    lv_obj_set_style_text_font(s_source, &jianshan_font_16, 0);
    lv_obj_set_style_text_color(s_source, lv_color_hex(0x62675F), 0);
    lv_obj_set_width(s_source, 170);
    lv_label_set_long_mode(s_source, LV_LABEL_LONG_WRAP);
    lv_obj_align(s_source, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    s_position = lv_label_create(panel);
    lv_obj_set_style_text_font(s_position, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_position, lv_color_hex(0x62675F), 0);
    lv_obj_align(s_position, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    jianshan_state_init(&s_state, jianshan_card_count,
                        esp_random() % jianshan_card_count);
    refresh_card();
    lv_screen_load(s_scr);
}

void demo_jianshan_exit(void)
{
    if (s_scr) {
        lv_obj_delete(s_scr);
        s_scr = s_title = s_body = s_source = s_position = s_battery = NULL;
    }
}

void demo_jianshan_key(bsp_btn_t btn, bsp_btn_ev_t ev)
{
    if (ev != BSP_BTN_CLICK) return;

    bool changed = false;
    if (btn == BSP_BTN_UP) changed = jianshan_state_older(&s_state);
    if (btn == BSP_BTN_DOWN) changed = jianshan_state_newer(&s_state);
    if (btn == BSP_BTN_OK) {
        jianshan_state_draw(&s_state, esp_random());
        changed = true;
    }
    if (changed) refresh_card();
}
