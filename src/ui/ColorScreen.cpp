#include "ColorScreen.h"
#include "AppUI.h"
#include <math.h>

#define CLR_BG  lv_color_hex(0x0D1B2A)

static constexpr int RING_SZ   = 260;
static constexpr int RING_W    = 52;
static constexpr int N_SEG     = 36;
static constexpr int RING_X    = (480 - RING_SZ) / 2;   // 110
static constexpr int RING_Y    = 72;
static constexpr int RING_MID_R = RING_SZ / 2 - RING_W / 2;  // 130 - 26 = 104

ColorScreen::ColorScreen(AppUI *ui) : ui_(ui) { build(); }

void ColorScreen::build() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_remove_flag(screen_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen_, CLR_BG, 0);
    lv_obj_set_style_bg_opa(screen_, LV_OPA_COVER, 0);

    // ── Back button ────────────────────────────────────────────────────────────
    lv_obj_t *backBtn = lv_button_create(screen_);
    lv_obj_set_size(backBtn, 100, 50);
    lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 12, 12);
    lv_obj_set_style_bg_color(backBtn, lv_color_hex(0x2C3E50), 0);
    lv_obj_set_style_radius(backBtn, 12, 0);
    lv_obj_add_event_cb(backBtn, onBackBtn, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(backBtn);
        lv_label_set_text(lbl, LV_SYMBOL_LEFT " Retour");
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);
    }

    // ── Title ──────────────────────────────────────────────────────────────────
    lv_obj_t *title = lv_label_create(screen_);
    lv_label_set_text(title, "Choisis une couleur");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // ── Hue ring: N_SEG arc segments (each 9°, 1° gap) ────────────────────────
    for (int i = 0; i < N_SEG; i++) {
        lv_obj_t *arc = lv_arc_create(screen_);
        lv_obj_set_size(arc, RING_SZ, RING_SZ);
        lv_obj_set_pos(arc, RING_X, RING_Y);
        lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);

        lv_arc_set_bg_start_angle(arc, i * 10);
        lv_arc_set_bg_end_angle(arc,   i * 10 + 9);
        lv_arc_set_range(arc, 0, 100);
        lv_arc_set_value(arc, 0);

        lv_color_t c = lv_color_hsv_to_rgb(i * 10, 100, 100);

        // Widget rectangle: fully transparent
        lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(arc, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(arc, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(arc, 0, LV_PART_MAIN);
        // Arc track = hue color
        lv_obj_set_style_arc_color(arc, c, LV_PART_MAIN);
        lv_obj_set_style_arc_width(arc, RING_W, LV_PART_MAIN);
        lv_obj_set_style_arc_opa(arc, LV_OPA_COVER, LV_PART_MAIN);
        // Indicator: hidden
        lv_obj_set_style_arc_opa(arc, LV_OPA_TRANSP, LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(arc, 0, LV_PART_INDICATOR);
        // Knob: hidden
        lv_obj_set_style_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    }

    // ── Center preview circle ──────────────────────────────────────────────────
    previewCirc_ = lv_obj_create(screen_);
    lv_obj_set_size(previewCirc_, 110, 110);
    lv_obj_set_pos(previewCirc_, RING_X + RING_SZ / 2 - 55, RING_Y + RING_SZ / 2 - 55);
    lv_obj_set_style_radius(previewCirc_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(previewCirc_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(previewCirc_, 3, 0);
    lv_obj_set_style_border_color(previewCirc_, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_border_opa(previewCirc_, LV_OPA_50, 0);
    lv_obj_set_style_shadow_width(previewCirc_, 15, 0);
    lv_obj_set_style_shadow_opa(previewCirc_, LV_OPA_60, 0);
    lv_obj_clear_flag(previewCirc_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(previewCirc_, LV_OBJ_FLAG_SCROLLABLE);

    // ── Cursor dot (tracks selected hue on the ring) ───────────────────────────
    cursor_ = lv_obj_create(screen_);
    lv_obj_set_size(cursor_, 22, 22);
    lv_obj_set_style_radius(cursor_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cursor_, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(cursor_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cursor_, 2, 0);
    lv_obj_set_style_border_color(cursor_, lv_color_make(60, 60, 60), 0);
    lv_obj_set_style_shadow_width(cursor_, 8, 0);
    lv_obj_set_style_shadow_color(cursor_, lv_color_white(), 0);
    lv_obj_set_style_shadow_opa(cursor_, LV_OPA_70, 0);
    lv_obj_clear_flag(cursor_, LV_OBJ_FLAG_CLICKABLE);

    // ── Touch overlay (transparent, on top of arcs) ────────────────────────────
    touchZone_ = lv_obj_create(screen_);
    lv_obj_set_size(touchZone_, RING_SZ, RING_SZ);
    lv_obj_set_pos(touchZone_, RING_X, RING_Y);
    lv_obj_set_style_bg_opa(touchZone_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(touchZone_, 0, 0);
    lv_obj_set_style_shadow_width(touchZone_, 0, 0);
    lv_obj_set_style_pad_all(touchZone_, 0, 0);
    lv_obj_clear_flag(touchZone_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(touchZone_, onTouchZone, LV_EVENT_PRESSING, this);
    lv_obj_add_event_cb(touchZone_, onTouchZone, LV_EVENT_CLICKED,  this);

    // ── Brightness slider ─────────────────────────────────────────────────────
    lv_obj_t *briLabel = lv_label_create(screen_);
    lv_label_set_text(briLabel, LV_SYMBOL_IMAGE " Luminosite");
    lv_obj_set_style_text_font(briLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(briLabel, lv_color_make(180, 180, 180), 0);
    lv_obj_align(briLabel, LV_ALIGN_BOTTOM_MID, 0, -68);

    briSlider_ = lv_slider_create(screen_);
    lv_obj_set_size(briSlider_, 300, 36);
    lv_obj_align(briSlider_, LV_ALIGN_BOTTOM_MID, 0, -24);
    lv_slider_set_range(briSlider_, 10, 254);
    lv_slider_set_value(briSlider_, currentBri_, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(briSlider_, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(briSlider_, LV_GRAD_DIR_HOR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(briSlider_, lv_color_white(), LV_PART_KNOB);
    lv_obj_add_event_cb(briSlider_, onBriChanged, LV_EVENT_VALUE_CHANGED, this);

    updatePreview();
    moveCursor(currentHue_);
}

void ColorScreen::updatePreview() {
    if (!previewCirc_) return;
    uint16_t hDeg = (uint32_t)currentHue_ * 360 / 65535;
    uint8_t  v    = (uint32_t)currentBri_ * 100 / 254;
    if (v < 5) v = 5;
    lv_color_t c     = lv_color_hsv_to_rgb(hDeg, 100, v);
    lv_color_t cFull = lv_color_hsv_to_rgb(hDeg, 100, 100);
    lv_obj_set_style_bg_color(previewCirc_, c, 0);
    lv_obj_set_style_shadow_color(previewCirc_, cFull, 0);

    // Brightness slider gradient: dark → current hue at full brightness
    if (briSlider_) {
        lv_obj_set_style_bg_grad_color(briSlider_, cFull, LV_PART_MAIN);
    }
}

void ColorScreen::moveCursor(uint16_t hue16) {
    if (!cursor_) return;
    float angleRad = (float)hue16 / 65535.0f * 2.0f * (float)M_PI;
    int cx = RING_X + RING_SZ / 2;
    int cy = RING_Y + RING_SZ / 2;
    int x  = cx + (int)(RING_MID_R * cosf(angleRad)) - 11;
    int y  = cy + (int)(RING_MID_R * sinf(angleRad)) - 11;
    lv_obj_set_pos(cursor_, x, y);
}

void ColorScreen::setInitialColor(HueColor color) {
    currentHue_ = color.hue;
    currentBri_ = color.bri;
    if (briSlider_) lv_slider_set_value(briSlider_, color.bri, LV_ANIM_OFF);
    updatePreview();
    moveCursor(currentHue_);
}

// ─── Callbacks ────────────────────────────────────────────────────────────────

void ColorScreen::onTouchZone(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));

    lv_indev_t *indev = lv_indev_active();
    if (!indev) return;

    lv_point_t pt;
    lv_indev_get_point(indev, &pt);

    lv_area_t area;
    lv_obj_get_coords(self->touchZone_, &area);
    int dx = pt.x - area.x1 - RING_SZ / 2;
    int dy = pt.y - area.y1 - RING_SZ / 2;
    int r  = (int)sqrtf((float)(dx * dx + dy * dy));

    // Accept touches only within the ring band (with a few px tolerance)
    if (r < RING_SZ / 2 - RING_W - 8 || r > RING_SZ / 2 + 4) return;

    float angle = atan2f((float)dy, (float)dx) * 180.0f / (float)M_PI;
    if (angle < 0.0f) angle += 360.0f;

    self->currentHue_ = (uint16_t)(angle / 360.0f * 65535.0f);
    self->currentBri_ = self->briSlider_
                        ? (uint8_t)lv_slider_get_value(self->briSlider_)
                        : 200;
    self->updatePreview();
    self->moveCursor(self->currentHue_);
    self->ui_->onColorPicked(self->currentHue_, 254, self->currentBri_);
}

void ColorScreen::onBriChanged(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));
    self->currentBri_ = (uint8_t)lv_slider_get_value(self->briSlider_);
    self->updatePreview();
    self->ui_->onColorPicked(self->currentHue_, 254, self->currentBri_);
}

void ColorScreen::onBackBtn(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));
    self->ui_->showHomeScreen();
}
