#include "demo.h"
#include "esp_random.h"
#include "jianshan_cards.h"
#include "lvgl.h"

LV_FONT_DECLARE(jianshan_sans_14);
LV_FONT_DECLARE(jianshan_serif_20);
LV_FONT_DECLARE(jianshan_serif_26);

#define JI_BLACK       0x070807
#define JI_BLACK_GLOW  0x171711
#define JI_GOLD        0xD0B273
#define JI_GOLD_DIM    0x786747
#define JI_IVORY       0xD6D0C4
#define JI_MUTED       0xA59E91
#define JI_RED         0x963F34

static lv_obj_t *s_scr;
static lv_obj_t *s_idle_view;
static lv_obj_t *s_draw_view;
static lv_obj_t *s_card_view;
static lv_obj_t *s_idle_ring;
static lv_obj_t *s_draw_ring;
static lv_obj_t *s_title;
static lv_obj_t *s_body;
static lv_obj_t *s_source;
static lv_obj_t *s_position;
static lv_obj_t *s_stamp;
static lv_obj_t *s_stamp_text;
static lv_timer_t *s_draw_timer;
static jianshan_state_t s_state;

typedef enum {
    JI_PHASE_IDLE,
    JI_PHASE_DRAWING,
    JI_PHASE_CARD,
} jianshan_phase_t;

static jianshan_phase_t s_phase;

static lv_obj_t *plain_obj(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    return obj;
}

static void set_view(lv_obj_t *visible)
{
    lv_obj_add_flag(s_idle_view, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_draw_view, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_card_view, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(visible, LV_OBJ_FLAG_HIDDEN);
}

static void set_opa(void *obj, int32_t value)
{
    lv_obj_set_style_opa(obj, (lv_opa_t)value, 0);
}

static void animate_opa(lv_obj_t *obj, uint32_t delay, uint32_t duration)
{
    lv_anim_delete(obj, set_opa);
    lv_obj_set_style_opa(obj, LV_OPA_TRANSP, 0);
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_exec_cb(&anim, set_opa);
    lv_anim_set_values(&anim, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_delay(&anim, delay);
    lv_anim_set_duration(&anim, duration);
    lv_anim_start(&anim);
}

static void start_pulse(lv_obj_t *obj, uint32_t duration)
{
    lv_anim_delete(obj, set_opa);
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_exec_cb(&anim, set_opa);
    lv_anim_set_values(&anim, LV_OPA_40, LV_OPA_COVER);
    lv_anim_set_duration(&anim, duration);
    lv_anim_set_playback_duration(&anim, duration);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_start(&anim);
}

static lv_obj_t *create_ring(lv_obj_t *parent, int x, int y, int size,
                             uint32_t color)
{
    lv_obj_t *ring = plain_obj(parent);
    lv_obj_set_pos(ring, x, y);
    lv_obj_set_size(ring, size, size);
    lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(ring, 1, 0);
    lv_obj_set_style_border_color(ring, lv_color_hex(color), 0);
    return ring;
}

static lv_obj_t *create_glow(lv_obj_t *parent, int x, int y, int size,
                             uint32_t color)
{
    lv_obj_t *glow = plain_obj(parent);
    lv_obj_set_pos(glow, x, y);
    lv_obj_set_size(glow, size, size);
    lv_obj_set_style_radius(glow, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(glow, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(glow, LV_OPA_COVER, 0);
    return glow;
}

static lv_obj_t *create_centered_label(lv_obj_t *parent, const char *text,
                                       int y, const lv_font_t *font,
                                       uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_width(label, 220);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, y);
    return label;
}

static void animate_card(void)
{
    lv_obj_set_style_opa(s_card_view, LV_OPA_COVER, 0);
    animate_opa(s_title, 80, 260);
    animate_opa(s_body, 230, 380);
    animate_opa(s_source, 410, 260);
    animate_opa(s_position, 410, 260);
    animate_opa(s_stamp, 630, 170);
}

static void refresh_card(bool animate)
{
    size_t index = jianshan_state_current(&s_state);
    const jianshan_card_t *card = &jianshan_cards[index];

    lv_label_set_text(s_title, card->title);
    lv_label_set_text(s_body, card->body);
    lv_label_set_text(s_source, card->source);
    lv_label_set_text(s_stamp_text, card->stamp);
    lv_label_set_text_fmt(s_position, "%02u / %02u",
                          (unsigned)(index + 1),
                          (unsigned)jianshan_card_count);

    lv_obj_set_style_text_font(s_title,
        card->quote ? &jianshan_sans_14 : &jianshan_serif_20, 0);
    lv_obj_set_style_text_letter_space(s_title, card->quote ? 2 : 0, 0);
    lv_obj_set_style_text_font(s_body,
        card->quote ? &jianshan_serif_26 : &jianshan_sans_14, 0);
    lv_obj_set_style_text_line_space(s_body, card->quote ? 11 : 6, 0);
    lv_obj_set_style_text_align(s_body,
        card->quote ? LV_TEXT_ALIGN_CENTER : LV_TEXT_ALIGN_LEFT, 0);

    set_view(s_card_view);
    s_phase = JI_PHASE_CARD;
    if (animate) animate_card();
    else {
        lv_obj_set_style_opa(s_card_view, LV_OPA_COVER, 0);
        lv_obj_set_style_opa(s_title, LV_OPA_COVER, 0);
        lv_obj_set_style_opa(s_body, LV_OPA_COVER, 0);
        lv_obj_set_style_opa(s_source, LV_OPA_COVER, 0);
        lv_obj_set_style_opa(s_position, LV_OPA_COVER, 0);
        lv_obj_set_style_opa(s_stamp, LV_OPA_COVER, 0);
    }
}

static void finish_draw(lv_timer_t *timer)
{
    (void)timer;
    s_draw_timer = NULL;
    refresh_card(true);
}

static void begin_draw(void)
{
    if (s_draw_timer) {
        lv_timer_delete(s_draw_timer);
        s_draw_timer = NULL;
    }
    jianshan_state_draw(&s_state, esp_random());
    set_view(s_draw_view);
    s_phase = JI_PHASE_DRAWING;
    start_pulse(s_draw_ring, 260);
    s_draw_timer = lv_timer_create(finish_draw, 900, NULL);
    lv_timer_set_repeat_count(s_draw_timer, 1);
}

static void build_idle(void)
{
    s_idle_view = plain_obj(s_scr);
    lv_obj_set_size(s_idle_view, 240, 320);
    create_centered_label(s_idle_view, "JIAN SHAN  ·  PRIVATE ARCHIVE", 26,
                          &lv_font_montserrat_10, JI_GOLD_DIM);
    s_idle_ring = create_ring(s_idle_view, 64, 76, 112, JI_GOLD_DIM);
    create_ring(s_idle_view, 78, 90, 84, 0x544728);
    lv_obj_t *emblem = create_ring(s_idle_view, 93, 105, 54, JI_GOLD);
    lv_obj_t *mountain = lv_label_create(emblem);
    lv_label_set_text(mountain, "山");
    lv_obj_set_style_text_font(mountain, &jianshan_serif_20, 0);
    lv_obj_set_style_text_color(mountain, lv_color_hex(JI_GOLD), 0);
    lv_obj_center(mountain);
    create_centered_label(s_idle_view, "PRESS OK", 224,
                          &lv_font_montserrat_14, JI_IVORY);
    lv_obj_t *sub = create_centered_label(s_idle_view, "DRAW FROM HISTORY", 255,
                                          &lv_font_montserrat_10, JI_GOLD_DIM);
    lv_obj_set_style_text_letter_space(sub, 1, 0);
    start_pulse(s_idle_ring, 1500);
}

static void build_drawing(void)
{
    s_draw_view = plain_obj(s_scr);
    lv_obj_set_size(s_draw_view, 240, 320);
    create_centered_label(s_draw_view, "JIAN SHAN  ·  DRAWING", 26,
                          &lv_font_montserrat_10, JI_GOLD_DIM);
    s_draw_ring = create_ring(s_draw_view, 61, 72, 118, JI_GOLD);
    create_ring(s_draw_view, 77, 88, 86, JI_GOLD_DIM);
    lv_obj_t *inner = create_ring(s_draw_view, 91, 102, 58, JI_GOLD_DIM);
    lv_obj_t *mountain = lv_label_create(inner);
    lv_label_set_text(mountain, "山");
    lv_obj_set_style_text_font(mountain, &jianshan_serif_20, 0);
    lv_obj_set_style_text_color(mountain, lv_color_hex(JI_GOLD), 0);
    lv_obj_center(mountain);
    create_centered_label(s_draw_view, "DRAWING...", 226,
                          &lv_font_montserrat_14, JI_IVORY);
    create_centered_label(s_draw_view, "HISTORY ANSWERS", 255,
                          &lv_font_montserrat_10, JI_GOLD_DIM);
}

static void build_card(void)
{
    s_card_view = plain_obj(s_scr);
    lv_obj_set_size(s_card_view, 240, 320);

    lv_obj_t *top = create_centered_label(s_card_view, "JIAN SHAN  ·  I", 12,
                                          &lv_font_montserrat_10, JI_GOLD_DIM);
    lv_obj_set_style_text_letter_space(top, 1, 0);
    create_ring(s_card_view, 77, 45, 86, 0x403822);
    lv_obj_t *emblem = create_ring(s_card_view, 97, 48, 46, JI_GOLD);
    lv_obj_t *mountain = lv_label_create(emblem);
    lv_label_set_text(mountain, "山");
    lv_obj_set_style_text_font(mountain, &jianshan_serif_20, 0);
    lv_obj_set_style_text_color(mountain, lv_color_hex(JI_GOLD), 0);
    lv_obj_center(mountain);

    s_title = lv_label_create(s_card_view);
    lv_obj_set_pos(s_title, 18, 105);
    lv_obj_set_width(s_title, 204);
    lv_obj_set_style_text_align(s_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(s_title, lv_color_hex(JI_GOLD), 0);

    s_body = lv_label_create(s_card_view);
    lv_obj_set_pos(s_body, 18, 143);
    lv_obj_set_width(s_body, 204);
    lv_label_set_long_mode(s_body, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(s_body, lv_color_hex(JI_IVORY), 0);

    s_source = lv_label_create(s_card_view);
    lv_obj_set_pos(s_source, 18, 275);
    lv_obj_set_size(s_source, 145, 36);
    lv_label_set_long_mode(s_source, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(s_source, &jianshan_sans_14, 0);
    lv_obj_set_style_text_color(s_source, lv_color_hex(JI_MUTED), 0);
    lv_obj_set_style_text_line_space(s_source, 1, 0);

    s_stamp = plain_obj(s_card_view);
    lv_obj_set_pos(s_stamp, 174, 272);
    lv_obj_set_size(s_stamp, 45, 27);
    lv_obj_set_style_border_width(s_stamp, 1, 0);
    lv_obj_set_style_border_color(s_stamp, lv_color_hex(JI_RED), 0);
    s_stamp_text = lv_label_create(s_stamp);
    lv_obj_set_style_text_font(s_stamp_text, &jianshan_sans_14, 0);
    lv_obj_set_style_text_color(s_stamp_text, lv_color_hex(JI_RED), 0);
    lv_obj_center(s_stamp_text);

    s_position = lv_label_create(s_card_view);
    lv_obj_set_style_text_font(s_position, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(s_position, lv_color_hex(JI_GOLD_DIM), 0);
    lv_obj_align(s_position, LV_ALIGN_BOTTOM_RIGHT, -20, -5);
}

void demo_jianshan_enter(void)
{
    s_scr = lv_obj_create(NULL);
    lv_obj_remove_flag(s_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(s_scr, lv_color_hex(JI_BLACK), 0);
    lv_obj_set_style_bg_grad_color(s_scr, lv_color_hex(JI_BLACK_GLOW), 0);
    lv_obj_set_style_bg_grad_dir(s_scr, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_border_width(s_scr, 0, 0);
    lv_obj_set_style_pad_all(s_scr, 0, 0);

    create_glow(s_scr, 35, 46, 170, 0x0D0E0B);
    create_glow(s_scr, 55, 66, 130, 0x12120D);
    create_glow(s_scr, 80, 91, 80, 0x1D1A10);

    lv_obj_t *frame = plain_obj(s_scr);
    lv_obj_set_pos(frame, 8, 8);
    lv_obj_set_size(frame, 224, 304);
    lv_obj_set_style_border_width(frame, 1, 0);
    lv_obj_set_style_border_color(frame, lv_color_hex(0x3D3525), 0);

    build_idle();
    build_drawing();
    build_card();
    jianshan_state_init(&s_state, jianshan_card_count,
                        esp_random() % jianshan_card_count);
    set_view(s_idle_view);
    s_phase = JI_PHASE_IDLE;
    lv_screen_load(s_scr);
}

void demo_jianshan_exit(void)
{
    if (s_draw_timer) {
        lv_timer_delete(s_draw_timer);
        s_draw_timer = NULL;
    }
    if (s_scr) lv_obj_delete(s_scr);
    s_scr = s_idle_view = s_draw_view = s_card_view = NULL;
    s_idle_ring = s_draw_ring = NULL;
    s_title = s_body = s_source = s_position = NULL;
    s_stamp = s_stamp_text = NULL;
}

void demo_jianshan_key(bsp_btn_t btn, bsp_btn_ev_t ev)
{
    if (ev != BSP_BTN_CLICK) return;
    if (btn == BSP_BTN_OK) {
        begin_draw();
        return;
    }
    if (s_phase != JI_PHASE_CARD) return;
    if (btn == BSP_BTN_UP && jianshan_state_older(&s_state)) refresh_card(false);
    if (btn == BSP_BTN_DOWN && jianshan_state_newer(&s_state)) refresh_card(false);
}
