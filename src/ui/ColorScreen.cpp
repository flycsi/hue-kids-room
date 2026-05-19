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

    // ── Hue slider ─────────────────────────────────────────────────────────────
    lv_obj_t *hueLabel = lv_label_create(screen_);
    lv_label_set_text(hueLabel, LV_SYMBOL_TINT " Couleur");
    lv_obj_set_style_text_font(hueLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(hueLabel, lv_color_make(180, 180, 180), 0);
    lv_obj_align(hueLabel, LV_ALIGN_CENTER, 0, -60);

    hueSlider_ = lv_slider_create(screen_);
    lv_obj_set_size(hueSlider_, 340, 36);
    lv_obj_align(hueSlider_, LV_ALIGN_CENTER, 0, -20);
    lv_slider_set_range(hueSlider_, 0, 359);
    lv_slider_set_value(hueSlider_, 30, LV_ANIM_OFF);
    // gradient: red → yellow → green → cyan → blue → magenta → red
    lv_obj_set_style_bg_grad_color(hueSlider_, lv_color_make(255, 0, 0), LV_PART_MAIN);
    lv_obj_set_style_bg_color(hueSlider_, lv_color_make(0, 255, 255), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(hueSlider_, LV_GRAD_DIR_HOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(hueSlider_, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hueSlider_, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_pad_all(hueSlider_, 6, LV_PART_KNOB);
    lv_obj_add_event_cb(hueSlider_, onColorChanged, LV_EVENT_VALUE_CHANGED, this);

    // ── Brightness slider ─────────────────────────────────────────────────────
    lv_obj_t *briLabel = lv_label_create(screen_);
    lv_label_set_text(briLabel, LV_SYMBOL_IMAGE " Luminosite");
    lv_obj_set_style_text_font(briLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(briLabel, lv_color_make(180, 180, 180), 0);
    lv_obj_align(briLabel, LV_ALIGN_BOTTOM_MID, 0, -70);

    briSlider_ = lv_slider_create(screen_);
    lv_obj_set_size(briSlider_, 340, 36);
    lv_obj_align(briSlider_, LV_ALIGN_BOTTOM_MID, 0, -24);
    lv_slider_set_range(briSlider_, 10, 254);
    lv_slider_set_value(briSlider_, 200, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(briSlider_, lv_color_hex(0xFFD700), LV_PART_KNOB);
    lv_obj_set_style_bg_grad_color(briSlider_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_color(briSlider_, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(briSlider_, LV_GRAD_DIR_HOR, LV_PART_MAIN);
    lv_obj_add_event_cb(briSlider_, onBriChanged, LV_EVENT_VALUE_CHANGED, this);
}

void ColorScreen::setInitialColor(HueColor color) {
    if (hueSlider_) {
        int hDeg = (uint32_t)color.hue * 359 / 65535;
        lv_slider_set_value(hueSlider_, hDeg, LV_ANIM_OFF);
    }
    if (briSlider_) lv_slider_set_value(briSlider_, color.bri, LV_ANIM_OFF);
}

// ─── Event callbacks ────────────────────────────────────────────────────────
void ColorScreen::onColorChanged(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));
    if (!self->hueSlider_) return;
    int  hDeg   = lv_slider_get_value(self->hueSlider_);
    uint16_t hueVal = (uint32_t)hDeg * 65535 / 359;
    uint8_t  briVal = self->briSlider_
                      ? (uint8_t)lv_slider_get_value(self->briSlider_)
                      : 200;
    self->ui_->onColorPicked(hueVal, 254, briVal);
}

void ColorScreen::onBriChanged(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));
    int  hDeg   = self->hueSlider_
                  ? lv_slider_get_value(self->hueSlider_)
                  : 30;
    uint16_t hueVal = (uint32_t)hDeg * 65535 / 359;
    uint8_t  briVal = (uint8_t)lv_slider_get_value(self->briSlider_);
    self->ui_->onColorPicked(hueVal, 254, briVal);
}

void ColorScreen::onBackBtn(lv_event_t *e) {
    auto *self = static_cast<ColorScreen *>(lv_event_get_user_data(e));
    self->ui_->showHomeScreen();
}
