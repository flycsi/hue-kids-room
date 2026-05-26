#include "PinScreen.h"
#include "AppUI.h"
#include "config.h"

#define CLR_BG      lv_color_hex(0x0D1B2A)
#define CLR_KEY     lv_color_hex(0x1E3A5F)
#define CLR_KEY_PR  lv_color_hex(0x0D1B2A)
#define CLR_DOT_OFF lv_color_hex(0x2A3A4A)
#define CLR_DOT_ON  lv_color_white()
#define CLR_ERROR   lv_color_hex(0xE53935)

PinScreen::PinScreen(AppUI *ui) : ui_(ui) {
    build();
}

void PinScreen::build() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_remove_flag(screen_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen_, CLR_BG, 0);
    lv_obj_set_style_bg_opa(screen_, LV_OPA_COVER, 0);
    lv_obj_set_user_data(screen_, this);

    // Title
    lv_obj_t *title = lv_label_create(screen_);
    lv_label_set_text(title, "Code Admin");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

    // 4 dot indicators
    for (int i = 0; i < 4; i++) {
        dots_[i] = lv_obj_create(screen_);
        lv_obj_set_size(dots_[i], 24, 24);
        lv_obj_set_style_radius(dots_[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dots_[i], CLR_DOT_OFF, 0);
        lv_obj_set_style_bg_opa(dots_[i], LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dots_[i], 2, 0);
        lv_obj_set_style_border_color(dots_[i], lv_color_make(80, 120, 160), 0);
        lv_obj_set_pos(dots_[i], 480/2 - 4*28/2 + i*28 + 2, 90);
    }

    // Error label (hidden by default)
    errorLabel_ = lv_label_create(screen_);
    lv_label_set_text(errorLabel_, "Code incorrect");
    lv_obj_set_style_text_font(errorLabel_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(errorLabel_, CLR_ERROR, 0);
    lv_obj_align(errorLabel_, LV_ALIGN_TOP_MID, 0, 132);
    lv_obj_add_flag(errorLabel_, LV_OBJ_FLAG_HIDDEN);

    // Numpad: 3×3 grid (1-9) + cancel + 0 + backspace
    const lv_coord_t KEY_W = 120, KEY_H = 80, HGAP = 20, VGAP = 16;
    const lv_coord_t PAD_X = (480 - 3*KEY_W - 2*HGAP) / 2;
    const lv_coord_t PAD_Y = 160;

    // digits 1-9 in a 3×3 grid
    for (int d = 1; d <= 9; d++) {
        int row = (d - 1) / 3;
        int col = (d - 1) % 3;
        lv_obj_t *btn = lv_button_create(screen_);
        lv_obj_set_size(btn, KEY_W, KEY_H);
        lv_obj_set_pos(btn, PAD_X + col*(KEY_W + HGAP), PAD_Y + row*(KEY_H + VGAP));
        lv_obj_set_style_bg_color(btn, CLR_KEY, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(btn, CLR_KEY_PR, LV_STATE_PRESSED);
        lv_obj_set_style_radius(btn, 16, 0);
        lv_obj_set_style_shadow_width(btn, 0, 0);

        lv_obj_t *lbl = lv_label_create(btn);
        char buf[2] = { (char)('0' + d), '\0' };
        lv_label_set_text(lbl, buf);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);

        // store digit as user data (cast to pointer)
        lv_obj_add_event_cb(btn, onKey, LV_EVENT_CLICKED, (void*)(intptr_t)d);
    }

    // Bottom row: [Annuler] [0] [⌫]
    lv_coord_t row3_y = PAD_Y + 3*(KEY_H + VGAP);

    // Cancel
    lv_obj_t *cancelBtn = lv_button_create(screen_);
    lv_obj_set_size(cancelBtn, KEY_W, KEY_H);
    lv_obj_set_pos(cancelBtn, PAD_X, row3_y);
    lv_obj_set_style_bg_color(cancelBtn, lv_color_hex(0x37474F), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cancelBtn, lv_color_hex(0x1C2A30), LV_STATE_PRESSED);
    lv_obj_set_style_radius(cancelBtn, 16, 0);
    lv_obj_set_style_shadow_width(cancelBtn, 0, 0);
    lv_obj_add_event_cb(cancelBtn, onCancel, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(cancelBtn);
        lv_label_set_text(lbl, "Annuler");
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl, lv_color_make(180, 180, 180), 0);
        lv_obj_center(lbl);
    }

    // 0
    lv_obj_t *btn0 = lv_button_create(screen_);
    lv_obj_set_size(btn0, KEY_W, KEY_H);
    lv_obj_set_pos(btn0, PAD_X + KEY_W + HGAP, row3_y);
    lv_obj_set_style_bg_color(btn0, CLR_KEY, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn0, CLR_KEY_PR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn0, 16, 0);
    lv_obj_set_style_shadow_width(btn0, 0, 0);
    lv_obj_add_event_cb(btn0, onKey, LV_EVENT_CLICKED, (void*)(intptr_t)0);
    {
        lv_obj_t *lbl = lv_label_create(btn0);
        lv_label_set_text(lbl, "0");
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);
    }

    // Backspace
    lv_obj_t *bsBtn = lv_button_create(screen_);
    lv_obj_set_size(bsBtn, KEY_W, KEY_H);
    lv_obj_set_pos(bsBtn, PAD_X + 2*(KEY_W + HGAP), row3_y);
    lv_obj_set_style_bg_color(bsBtn, CLR_KEY, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bsBtn, CLR_KEY_PR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(bsBtn, 16, 0);
    lv_obj_set_style_shadow_width(bsBtn, 0, 0);
    lv_obj_add_event_cb(bsBtn, onBackspace, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(bsBtn);
        lv_label_set_text(lbl, LV_SYMBOL_BACKSPACE);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);
    }
}

void PinScreen::reset() {
    len_ = 0;
    entered_[0] = '\0';
    lv_obj_add_flag(errorLabel_, LV_OBJ_FLAG_HIDDEN);
    updateDots();
}

void PinScreen::updateDots() {
    for (int i = 0; i < 4; i++) {
        lv_obj_set_style_bg_color(dots_[i],
            i < len_ ? CLR_DOT_ON : CLR_DOT_OFF, 0);
    }
}

void PinScreen::pushDigit(char d) {
    if (len_ >= 4) return;
    entered_[len_++] = d;
    entered_[len_]   = '\0';
    updateDots();
    if (len_ == 4) validate();
}

void PinScreen::backspace() {
    if (len_ > 0) {
        entered_[--len_] = '\0';
        updateDots();
    }
}

void PinScreen::validate() {
    int code = atoi(entered_);
    if (code == ADMIN_PIN_CODE) {
        reset();
        ui_->showAdminScreen();
    } else {
        showError();
    }
}

void PinScreen::showError() {
    lv_obj_remove_flag(errorLabel_, LV_OBJ_FLAG_HIDDEN);
    if (errorTimer_) {
        lv_timer_reset(errorTimer_);
    } else {
        errorTimer_ = lv_timer_create(onErrorTimer, 1500, this);
        lv_timer_set_repeat_count(errorTimer_, 1);
    }
}

// ─── Static callbacks ──────────────────────────────────────────────────────

void PinScreen::onKey(lv_event_t *e) {
    // Find the PinScreen instance by walking up to screen_ owner
    // We stored this pointer on the screen_ object; key buttons use digit as user_data
    lv_obj_t *btn = static_cast<lv_obj_t *>(lv_event_get_target(e));
    lv_obj_t *scr = lv_obj_get_screen(btn);
    auto *self = static_cast<PinScreen *>(lv_obj_get_user_data(scr));
    if (!self) return;
    int digit = (int)(intptr_t)lv_event_get_user_data(e);
    self->pushDigit('0' + digit);
}

void PinScreen::onBackspace(lv_event_t *e) {
    auto *self = static_cast<PinScreen *>(lv_event_get_user_data(e));
    self->backspace();
}

void PinScreen::onCancel(lv_event_t *e) {
    auto *self = static_cast<PinScreen *>(lv_event_get_user_data(e));
    self->reset();
    self->ui_->showHomeScreen();
}

void PinScreen::onErrorTimer(lv_timer_t *t) {
    auto *self = static_cast<PinScreen *>(lv_timer_get_user_data(t));
    self->errorTimer_ = nullptr;
    self->reset();
}
