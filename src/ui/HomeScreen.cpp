#include "HomeScreen.h"
#include "AppUI.h"

// Palette de couleurs des boutons
#define CLR_NIGHT   lv_color_hex(0xF5A623)  // amber warm
#define CLR_PARTY   lv_color_hex(0xE040FB)  // purple
#define CLR_POWER   lv_color_hex(0x4CAF50)  // green
#define CLR_BG      lv_color_hex(0x0D1B2A)  // dark navy
#define CLR_CIRCLE  lv_color_hex(0x5B9BD5)  // default blue

HomeScreen::HomeScreen(AppUI *ui) : ui_(ui) {
    build();
}

void HomeScreen::styleBtn(lv_obj_t *btn, lv_color_t color) {
    lv_obj_set_style_bg_color(btn, color, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, lv_color_darken(color, 50), LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn, 20, 0);
    lv_obj_set_style_shadow_width(btn, 6, 0);
    lv_obj_set_style_shadow_ofs_y(btn, 3, 0);
    lv_obj_set_style_shadow_color(btn, lv_color_black(), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_30, 0);
}

void HomeScreen::build() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_remove_flag(screen_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen_, CLR_BG, 0);
    lv_obj_set_style_bg_opa(screen_, LV_OPA_COVER, 0);

    const lv_coord_t W = 480, H = 480;

    // ── Title ──────────────────────────────────────────────────────────────────
    lv_obj_t *title = lv_label_create(screen_);
    lv_label_set_text(title, "Ma Chambre");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 16);

    // ── Big color circle (tap → color screen) ─────────────────────────────────
    colorCircle_ = lv_obj_create(screen_);
    lv_obj_set_size(colorCircle_, 200, 200);
    lv_obj_align(colorCircle_, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_style_radius(colorCircle_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(colorCircle_, CLR_CIRCLE, 0);
    lv_obj_set_style_bg_opa(colorCircle_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(colorCircle_, 4, 0);
    lv_obj_set_style_border_color(colorCircle_, lv_color_white(), 0);
    lv_obj_set_style_border_opa(colorCircle_, LV_OPA_40, 0);
    lv_obj_set_style_shadow_width(colorCircle_, 20, 0);
    lv_obj_set_style_shadow_color(colorCircle_, CLR_CIRCLE, 0);
    lv_obj_set_style_shadow_opa(colorCircle_, LV_OPA_60, 0);
    lv_obj_clear_flag(colorCircle_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(colorCircle_, onColorCircleTap, LV_EVENT_CLICKED, this);

    // Color circle label
    lv_obj_t *circleHint = lv_label_create(colorCircle_);
    lv_label_set_text(circleHint, "Couleur");
    lv_obj_set_style_text_font(circleHint, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(circleHint, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_opa(circleHint, LV_OPA_70, 0);
    lv_obj_align(circleHint, LV_ALIGN_BOTTOM_MID, 0, -12);

    // ── 3 Mode buttons ────────────────────────────────────────────────────────
    const lv_coord_t BTN_W = 130, BTN_H = 90;
    const lv_coord_t BTN_Y = H - BTN_H - 30;
    const lv_coord_t GAP   = (W - 3 * BTN_W) / 4;

    // Night button
    nightBtn_ = lv_button_create(screen_);
    lv_obj_set_size(nightBtn_, BTN_W, BTN_H);
    lv_obj_set_pos(nightBtn_, GAP, BTN_Y);
    styleBtn(nightBtn_, CLR_NIGHT);
    lv_obj_add_event_cb(nightBtn_, onNightBtn, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(nightBtn_);
        lv_label_set_text(lbl, "Nuit");
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -8);
        lv_obj_t *ico = lv_label_create(nightBtn_);
        lv_label_set_text(ico, LV_SYMBOL_EYE_CLOSE);
        lv_obj_set_style_text_font(ico, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(ico, lv_color_white(), 0);
        lv_obj_align(ico, LV_ALIGN_TOP_MID, 0, 8);
    }

    // Party button
    partyBtn_ = lv_button_create(screen_);
    lv_obj_set_size(partyBtn_, BTN_W, BTN_H);
    lv_obj_set_pos(partyBtn_, GAP * 2 + BTN_W, BTN_Y);
    styleBtn(partyBtn_, CLR_PARTY);
    lv_obj_add_event_cb(partyBtn_, onPartyBtn, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(partyBtn_);
        lv_label_set_text(lbl, "Fete");
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -8);
        lv_obj_t *ico = lv_label_create(partyBtn_);
        lv_label_set_text(ico, LV_SYMBOL_BELL);
        lv_obj_set_style_text_font(ico, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(ico, lv_color_white(), 0);
        lv_obj_align(ico, LV_ALIGN_TOP_MID, 0, 8);
    }

    // Power button
    powerBtn_ = lv_button_create(screen_);
    lv_obj_set_size(powerBtn_, BTN_W, BTN_H);
    lv_obj_set_pos(powerBtn_, GAP * 3 + BTN_W * 2, BTN_Y);
    styleBtn(powerBtn_, CLR_POWER);
    lv_obj_add_event_cb(powerBtn_, onPowerBtn, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(powerBtn_);
        lv_label_set_text(lbl, "On/Off");
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -8);
        lv_obj_t *ico = lv_label_create(powerBtn_);
        lv_label_set_text(ico, LV_SYMBOL_POWER);
        lv_obj_set_style_text_font(ico, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(ico, lv_color_white(), 0);
        lv_obj_align(ico, LV_ALIGN_TOP_MID, 0, 8);
    }

    // ── Status label (bottom) ─────────────────────────────────────────────────
    statusLabel_ = lv_label_create(screen_);
    lv_label_set_text(statusLabel_, "Connexion...");
    lv_obj_set_style_text_font(statusLabel_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(statusLabel_, lv_color_make(120, 120, 120), 0);
    lv_obj_align(statusLabel_, LV_ALIGN_TOP_MID, 0, H - 24);
}

void HomeScreen::updateColorPreview(HueColor color, AppMode mode) {
    if (!colorCircle_) return;

    lv_color_t c;
    if (!color.on) {
        c = lv_color_make(50, 50, 50);
    } else if (mode == AppMode::Night) {
        c = lv_color_make(255, 160, 60);
    } else if (mode == AppMode::Party) {
        // Animate: rotate hue around the circle
        static uint8_t animStep = 0;
        animStep = (animStep + 30) % 360;
        c = lv_color_hsv_to_rgb(animStep, 100, 100);
    } else {
        uint16_t h = (uint32_t)color.hue * 360 / 65535;
        uint8_t  s = (uint32_t)color.sat * 100 / 254;
        uint8_t  v = (uint32_t)color.bri * 100 / 254;
        c = lv_color_hsv_to_rgb(h, s, v);
    }

    lv_obj_set_style_bg_color(colorCircle_, c, 0);
    lv_obj_set_style_shadow_color(colorCircle_, c, 0);
}

void HomeScreen::setStatus(const char *msg) {
    if (statusLabel_) lv_label_set_text(statusLabel_, msg);
}

// ─── Event callbacks ────────────────────────────────────────────────────────
void HomeScreen::onColorCircleTap(lv_event_t *e) {
    auto *self = static_cast<HomeScreen *>(lv_event_get_user_data(e));
    self->ui_->showColorScreen();
}
void HomeScreen::onNightBtn(lv_event_t *e) {
    auto *self = static_cast<HomeScreen *>(lv_event_get_user_data(e));
    self->ui_->onNightMode();
}
void HomeScreen::onPartyBtn(lv_event_t *e) {
    auto *self = static_cast<HomeScreen *>(lv_event_get_user_data(e));
    self->ui_->onPartyMode();
}
void HomeScreen::onPowerBtn(lv_event_t *e) {
    auto *self = static_cast<HomeScreen *>(lv_event_get_user_data(e));
    self->ui_->onPowerToggle();
}
