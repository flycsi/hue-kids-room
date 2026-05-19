#include "ColorScreen.h"
#include "AppUI.h"

#define CLR_BG lv_color_hex(0x0D1B2A)

ColorScreen::ColorScreen(AppUI *ui) : ui_(ui) {
    build();
}

void ColorScreen::build() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen_, CLR_BG, 0);
    lv_obj_set_style_bg_opa(screen_, LV_OPA_COVER, 0);

    // ── Back button ────────────────────────────────────────────────────────────
    lv_obj_t *backBtn = lv_btn_create(screen_);
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

    // ── Color wheel ────────────────────────────────────────────────────────────
#if LV_USE_COLORWHEEL
    colorWheel_ = lv_colorwheel_create(screen_, true);
    lv_obj_set_size(colorWheel_, 300, 300);
    lv_obj_align(colorWheel_, LV_ALIGN_CENTER, 0, -20);
    lv_obj_add_event_cb(colorWheel_, onColorChanged, LV_EVENT_VALUE_CHANGED, this);
#else
    // Fallback: arc-based hue slider if colorwheel not enabled
    lv_obj_t *msg = lv_label_create(screen_);
    lv_label_set_text(msg, "Activez LV_USE_COLORWHEEL\ndans lv_conf.h");
    lv_obj_set_style_text_color(msg, lv_color_white(), 0);
    lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
#endif

    // ── Brightness slider ─────────────────────────────────────────────────────
    lv_obj_t *briLabel = lv_label_create(screen_);
    lv_label_set_text(briLabel, LV_SYMBOL_BRIGHTNESS_MEDIUM " Luminosite");
    lv_obj_set_style_text_font(briLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(briLabel, lv_color_make(180, 180, 180), 0);
    lv_obj_align(briLabel, LV_ALIGN_BOTTOM_MID, 0, -60);

    briSlider_ = lv_slider_create(screen_);
    lv_obj_set_size(briSlider_, 340, 30);
    lv_obj_align(briSlider_, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_slider_set_range(briSlider_, 10, 254);
    lv_slider_set_value(briSlider_, 200, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(briSlider_, lv_color_hex(0xFFD700), LV_PART_KNOB);
    lv_obj_set_style_bg_color(briSlider_, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR);
    lv_obj_add_event_cb(briSlider_, onBriChanged, LV_EVENT_VALUE_CHANGED, this);
}

void ColorScreen::setInitialColor(HueColor color) {
#if LV_USE_COLORWHEEL
    if (!colorWheel_) return;
    uint16_t h = (uint32_t)color.hue * 360 / 65535;
    uint8_t  s = (uint32_t)color.sat * 100 / 254;
    uint8_t  v = (uint32_t)color.bri * 100 / 254;
    lv_colorwheel_set_hsv(colorWheel_, {(uint16_t)h, s, v});
#endif
    if (briSlider_) lv_slider_set_value(briSlider_, color.bri, LV_ANIM_OFF);
}

// ─── Event callbacks ────────────────────────────────────────────────────────
void ColorScreen::onColorChanged(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));
#if LV_USE_COLORWHEEL
    lv_color_hsv_t hsv = lv_colorwheel_get_hsv(self->colorWheel_);
    uint16_t hueVal    = (uint32_t)hsv.h * 65535 / 360;
    uint8_t  satVal    = (uint32_t)hsv.s * 254 / 100;
    uint8_t  briVal    = self->briSlider_ ? (uint8_t)lv_slider_get_value(self->briSlider_) : 200;
    self->ui_->onColorPicked(hueVal, satVal, briVal);
#endif
}

void ColorScreen::onBriChanged(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));
#if LV_USE_COLORWHEEL
    if (!self->colorWheel_) return;
    lv_color_hsv_t hsv = lv_colorwheel_get_hsv(self->colorWheel_);
    uint16_t hueVal    = (uint32_t)hsv.h * 65535 / 360;
    uint8_t  satVal    = (uint32_t)hsv.s * 254 / 100;
#else
    uint16_t hueVal = 43690; uint8_t satVal = 200;
#endif
    uint8_t briVal = (uint8_t)lv_slider_get_value(self->briSlider_);
    self->ui_->onColorPicked(hueVal, satVal, briVal);
}

void ColorScreen::onBackBtn(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));
    self->ui_->showHomeScreen();
}
