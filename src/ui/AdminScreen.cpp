#include "AdminScreen.h"
#include "AppUI.h"
#include "config.h"

#define CLR_BG      lv_color_hex(0x0D1B2A)
#define CLR_BTN     lv_color_hex(0x1E3A5F)
#define CLR_BTN_PR  lv_color_hex(0x0D1B2A)
#define CLR_BACK    lv_color_hex(0x37474F)
#define CLR_BACK_PR lv_color_hex(0x1C2A30)

AdminScreen::AdminScreen(AppUI *ui) : ui_(ui) {
    build();
}

void AdminScreen::build() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_remove_flag(screen_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen_, CLR_BG, 0);
    lv_obj_set_style_bg_opa(screen_, LV_OPA_COVER, 0);

    // Title
    lv_obj_t *title = lv_label_create(screen_);
    lv_label_set_text(title, "Administration");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

    // Section label
    lv_obj_t *section = lv_label_create(screen_);
    lv_label_set_text(section, "Duree timer Dodo");
    lv_obj_set_style_text_font(section, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(section, lv_color_make(150, 180, 210), 0);
    lv_obj_align(section, LV_ALIGN_CENTER, 0, -80);

    // [−] button
    lv_obj_t *minusBtn = lv_button_create(screen_);
    lv_obj_set_size(minusBtn, 90, 90);
    lv_obj_align(minusBtn, LV_ALIGN_CENTER, -120, 0);
    lv_obj_set_style_bg_color(minusBtn, CLR_BTN, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(minusBtn, CLR_BTN_PR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(minusBtn, 20, 0);
    lv_obj_set_style_shadow_width(minusBtn, 0, 0);
    lv_obj_add_event_cb(minusBtn, onMinus, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(minusBtn);
        lv_label_set_text(lbl, LV_SYMBOL_MINUS);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);
    }

    // Duration value label
    durationLbl_ = lv_label_create(screen_);
    lv_obj_set_style_text_font(durationLbl_, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(durationLbl_, lv_color_white(), 0);
    lv_obj_align(durationLbl_, LV_ALIGN_CENTER, 0, 0);
    updateDurationLabel();

    // Unit label
    lv_obj_t *unitLbl = lv_label_create(screen_);
    lv_label_set_text(unitLbl, "min");
    lv_obj_set_style_text_font(unitLbl, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(unitLbl, lv_color_make(150, 180, 210), 0);
    lv_obj_align(unitLbl, LV_ALIGN_CENTER, 0, 50);

    // [+] button
    lv_obj_t *plusBtn = lv_button_create(screen_);
    lv_obj_set_size(plusBtn, 90, 90);
    lv_obj_align(plusBtn, LV_ALIGN_CENTER, 120, 0);
    lv_obj_set_style_bg_color(plusBtn, CLR_BTN, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(plusBtn, CLR_BTN_PR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(plusBtn, 20, 0);
    lv_obj_set_style_shadow_width(plusBtn, 0, 0);
    lv_obj_add_event_cb(plusBtn, onPlus, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(plusBtn);
        lv_label_set_text(lbl, LV_SYMBOL_PLUS);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);
    }

    // Brightness settings button
    lv_obj_t *briBtn = lv_button_create(screen_);
    lv_obj_set_size(briBtn, 280, 55);
    lv_obj_align(briBtn, LV_ALIGN_BOTTOM_MID, 0, -90);
    lv_obj_set_style_bg_color(briBtn, lv_color_hex(0x0D3B5E), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(briBtn, CLR_BTN_PR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(briBtn, 16, 0);
    lv_obj_set_style_shadow_width(briBtn, 0, 0);
    lv_obj_add_event_cb(briBtn, onBrightness, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(briBtn);
        lv_label_set_text(lbl, LV_SYMBOL_IMAGE "  Luminosite " LV_SYMBOL_RIGHT);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);
    }

    // Back button
    lv_obj_t *backBtn = lv_button_create(screen_);
    lv_obj_set_size(backBtn, 200, 55);
    lv_obj_align(backBtn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(backBtn, CLR_BACK, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(backBtn, CLR_BACK_PR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(backBtn, 16, 0);
    lv_obj_set_style_shadow_width(backBtn, 0, 0);
    lv_obj_add_event_cb(backBtn, onBack, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(backBtn);
        lv_label_set_text(lbl, LV_SYMBOL_LEFT " Retour");
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);
    }
}

void AdminScreen::updateDurationLabel() {
    uint32_t mins = ui_->hue()->getSleepDurationMs() / 60000;
    char buf[8];
    snprintf(buf, sizeof(buf), "%lu", mins);
    if (durationLbl_) lv_label_set_text(durationLbl_, buf);
}

void AdminScreen::refresh() {
    updateDurationLabel();
}

// ─── Callbacks ─────────────────────────────────────────────────────────────

void AdminScreen::onMinus(lv_event_t *e) {
    auto *self = static_cast<AdminScreen *>(lv_event_get_user_data(e));
    uint32_t mins = self->ui_->hue()->getSleepDurationMs() / 60000;
    if (mins > SLEEP_DURATION_MIN_MIN) {
        mins -= 5;
        self->ui_->hue()->setSleepDuration(mins * 60000UL);
        self->updateDurationLabel();
    }
}

void AdminScreen::onPlus(lv_event_t *e) {
    auto *self = static_cast<AdminScreen *>(lv_event_get_user_data(e));
    uint32_t mins = self->ui_->hue()->getSleepDurationMs() / 60000;
    if (mins < SLEEP_DURATION_MAX_MIN) {
        mins += 5;
        self->ui_->hue()->setSleepDuration(mins * 60000UL);
        self->updateDurationLabel();
    }
}

void AdminScreen::onBrightness(lv_event_t *e) {
    auto *self = static_cast<AdminScreen *>(lv_event_get_user_data(e));
    self->ui_->showBrightnessScreen();
}

void AdminScreen::onBack(lv_event_t *e) {
    auto *self = static_cast<AdminScreen *>(lv_event_get_user_data(e));
    self->ui_->showHomeScreen();
}
