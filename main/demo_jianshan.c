#include "demo.h"
#include "esp_random.h"
#include "jianshan_cards.h"
#include "lvgl.h"
#include "ui_pixel.h"

LV_FONT_DECLARE(jianshan_sans_14);
LV_FONT_DECLARE(jianshan_sans_18);
LV_FONT_DECLARE(jianshan_serif_20);
LV_FONT_DECLARE(jianshan_serif_26);

static lv_obj_t *s_scr;
static lv_obj_t *s_title;
static lv_obj_t *s_body;
static lv_obj_t *s_source;
static lv_obj_t *s_position;
static jianshan_state_t s_state;

static void refresh_card(void)
{
    size_t index = jianshan_state_current(&s_state);
    const jianshan_card_t *card = &jianshan_cards[index];

    lv_label_set_text(s_title, card->title);
    lv_label_set_text(s_body, card->body);
    lv_label_set_text(s_source, card->source);
    lv_label_set_text_fmt(s_position, "%02u / %02u",
                          (unsigned)(index + 1),
                          (unsigned)jianshan_card_count);

    lv_obj_set_style_text_font(s_title,
        card->quote ? &jianshan_sans_14 : &jianshan_serif_20, 0);
    lv_obj_set_style_text_color(s_title,
        lv_color_hex(card->quote ? UI_RED : UI_INK), 0);
    lv_obj_set_style_text_letter_space(s_title, card->quote ? 2 : 0, 0);

    lv_obj_set_style_text_font(s_body,
        card->quote ? &jianshan_serif_26 : &jianshan_sans_18, 0);
    lv_obj_set_style_text_line_space(s_body, card->quote ? 12 : 6, 0);
    lv_obj_update_layout(s_body);
    lv_obj_align(s_body, LV_ALIGN_CENTER, 0, 8);
}

void demo_jianshan_enter(void)
{
    s_scr = lv_obj_create(NULL);
    lv_obj_remove_flag(s_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(s_scr, lv_color_hex(UI_PAPER), 0);
    lv_obj_set_style_border_width(s_scr, 0, 0);
    lv_obj_set_style_pad_all(s_scr, 0, 0);

    lv_obj_t *mark = lv_obj_create(s_scr);
    lv_obj_remove_flag(mark, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(mark, 18, 16);
    lv_obj_set_size(mark, 43, 25);
    lv_obj_set_style_radius(mark, 0, 0);
    lv_obj_set_style_border_width(mark, 1, 0);
    lv_obj_set_style_border_color(mark, lv_color_hex(UI_RED), 0);
    lv_obj_set_style_bg_opa(mark, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(mark, 0, 0);
    lv_obj_t *mark_text = lv_label_create(mark);
    lv_label_set_text(mark_text, "见山");
    lv_obj_set_style_text_font(mark_text, &jianshan_sans_14, 0);
    lv_obj_set_style_text_color(mark_text, lv_color_hex(UI_RED), 0);
    lv_obj_center(mark_text);

    s_position = lv_label_create(s_scr);
    lv_obj_set_style_text_font(s_position, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_position, lv_color_hex(0x65695D), 0);
    lv_obj_align(s_position, LV_ALIGN_TOP_RIGHT, -18, 20);

    s_title = lv_label_create(s_scr);
    lv_obj_set_pos(s_title, 18, 62);
    lv_obj_set_width(s_title, 204);

    s_body = lv_label_create(s_scr);
    lv_obj_set_style_text_color(s_body, lv_color_hex(UI_INK), 0);
    lv_obj_set_width(s_body, 204);
    lv_label_set_long_mode(s_body, LV_LABEL_LONG_WRAP);

    lv_obj_t *rule = lv_obj_create(s_scr);
    lv_obj_remove_flag(rule, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(rule, 18, 259);
    lv_obj_set_size(rule, 204, 1);
    lv_obj_set_style_border_width(rule, 0, 0);
    lv_obj_set_style_bg_color(rule, lv_color_hex(0xCCC9B9), 0);
    lv_obj_set_style_pad_all(rule, 0, 0);

    s_source = lv_label_create(s_scr);
    lv_obj_set_style_text_font(s_source, &jianshan_sans_14, 0);
    lv_obj_set_style_text_color(s_source, lv_color_hex(0x65695D), 0);
    lv_obj_set_style_text_line_space(s_source, 2, 0);
    lv_obj_set_pos(s_source, 18, 271);
    lv_obj_set_width(s_source, 204);
    lv_label_set_long_mode(s_source, LV_LABEL_LONG_WRAP);

    jianshan_state_init(&s_state, jianshan_card_count,
                        esp_random() % jianshan_card_count);
    refresh_card();
    lv_screen_load(s_scr);
}

void demo_jianshan_exit(void)
{
    if (s_scr) {
        lv_obj_delete(s_scr);
        s_scr = s_title = s_body = s_source = s_position = NULL;
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
