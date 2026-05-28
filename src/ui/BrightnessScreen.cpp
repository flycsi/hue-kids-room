#include "BrightnessScreen.h"
#include "AppUI.h"
#include "config.h"
#include "bsp_lvgl_port.h"

#define CLR_BG      lv_color_hex(0x0D1B2A)
#define CLR_BTN     lv_color_hex(0x1E3A5F)
#define CLR_BTN_ACT lv_color_hex(0x1565C0)
#define CLR_BTN_PR  lv_color_hex(0x0D1B2A)
#define CLR_BACK    lv_color_hex(0x37474F)

// Retrieve BrightnessScreen* from any child widget
static BrightnessScreen *selfFromEvent(lv_event_t *e) {
    lv_obj_t *tgt = static_cast<lv_obj_t *>(lv_event_get_target(e));
    return static_cast<BrightnessScreen *>(lv_obj_get_user_data(lv_obj_get_screen(tgt)));
}

BrightnessScreen::BrightnessScreen(AppUI *ui) : ui_(ui) { build(); }

void BrightnessScreen::build() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_user_data(screen_, this);
    lv_obj_remove_flag(screen_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen_, CLR_BG, 0);
    lv_obj_set_style_bg_opa(screen_, LV_OPA_COVER, 0);

    // ── Title ───────────────────────────────────────────────────────────────
    lv_obj_t *title = lv_label_create(screen_);
    lv_label_set_text(title, "Luminosite");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // ── Mode toggle [Auto] [Manuel] ─────────────────────────────────────────
    const lv_coord_t TW = 200, TH = 55, TGAP = 16;
    const lv_coord_t TX = (480 - 2*TW - TGAP) / 2;  // = 32

    autoBtn_ = lv_button_create(screen_);
    lv_obj_set_size(autoBtn_, TW, TH);
    lv_obj_set_pos(autoBtn_, TX, 72);
    lv_obj_set_style_radius(autoBtn_, 12, 0);
    lv_obj_set_style_shadow_width(autoBtn_, 0, 0);
    lv_obj_set_style_bg_color(autoBtn_, CLR_BTN_PR, LV_STATE_PRESSED);
    lv_obj_add_event_cb(autoBtn_, onAutoToggle, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *l = lv_label_create(autoBtn_);
        lv_label_set_text(l, "Auto");
        lv_obj_set_style_text_font(l, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(l, lv_color_white(), 0);
        lv_obj_center(l);
    }

    manualBtn_ = lv_button_create(screen_);
    lv_obj_set_size(manualBtn_, TW, TH);
    lv_obj_set_pos(manualBtn_, TX + TW + TGAP, 72);
    lv_obj_set_style_radius(manualBtn_, 12, 0);
    lv_obj_set_style_shadow_width(manualBtn_, 0, 0);
    lv_obj_set_style_bg_color(manualBtn_, CLR_BTN_PR, LV_STATE_PRESSED);
    lv_obj_add_event_cb(manualBtn_, onManualToggle, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *l = lv_label_create(manualBtn_);
        lv_label_set_text(l, "Manuel");
        lv_obj_set_style_text_font(l, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(l, lv_color_white(), 0);
        lv_obj_center(l);
    }

    // ── Auto panel: 3 rows (Jour / Soir / Nuit) ─────────────────────────────
    autoPanel_ = lv_obj_create(screen_);
    lv_obj_set_size(autoPanel_, 480, 210);
    lv_obj_set_pos(autoPanel_, 0, 148);
    lv_obj_set_style_bg_opa(autoPanel_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(autoPanel_, 0, 0);
    lv_obj_set_style_pad_all(autoPanel_, 0, 0);
    lv_obj_remove_flag(autoPanel_, LV_OBJ_FLAG_SCROLLABLE);

    struct { const char *label; int y; int field; } rows[3] = {
        { "Jour   7h-20h",   5, 0 },
        { "Soir  20h-22h",  73, 1 },
        { "Nuit  22h-7h",  141, 2 },
    };

    for (auto &row : rows) {
        // Period label
        lv_obj_t *lbl = lv_label_create(autoPanel_);
        lv_label_set_text(lbl, row.label);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl, lv_color_make(140, 190, 235), 0);
        lv_obj_set_pos(lbl, 20, row.y + 19);

        // [−]
        lv_obj_t *minBtn = lv_button_create(autoPanel_);
        lv_obj_set_size(minBtn, 58, 52);
        lv_obj_set_pos(minBtn, 228, row.y + 7);
        lv_obj_set_style_bg_color(minBtn, CLR_BTN, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(minBtn, CLR_BTN_PR, LV_STATE_PRESSED);
        lv_obj_set_style_radius(minBtn, 10, 0);
        lv_obj_set_style_shadow_width(minBtn, 0, 0);
        lv_obj_add_event_cb(minBtn, onMinus, LV_EVENT_CLICKED, (void *)(intptr_t)row.field);
        { lv_obj_t *i = lv_label_create(minBtn); lv_label_set_text(i, LV_SYMBOL_MINUS);
          lv_obj_set_style_text_font(i, &lv_font_montserrat_24, 0);
          lv_obj_set_style_text_color(i, lv_color_white(), 0); lv_obj_center(i); }

        // Value label
        valueLbls_[row.field] = lv_label_create(autoPanel_);
        lv_obj_set_style_text_font(valueLbls_[row.field], &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(valueLbls_[row.field], lv_color_white(), 0);
        lv_obj_set_pos(valueLbls_[row.field], 295, row.y + 17);

        // [+]
        lv_obj_t *plsBtn = lv_button_create(autoPanel_);
        lv_obj_set_size(plsBtn, 58, 52);
        lv_obj_set_pos(plsBtn, 362, row.y + 7);
        lv_obj_set_style_bg_color(plsBtn, CLR_BTN, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(plsBtn, CLR_BTN_PR, LV_STATE_PRESSED);
        lv_obj_set_style_radius(plsBtn, 10, 0);
        lv_obj_set_style_shadow_width(plsBtn, 0, 0);
        lv_obj_add_event_cb(plsBtn, onPlus, LV_EVENT_CLICKED, (void *)(intptr_t)row.field);
        { lv_obj_t *i = lv_label_create(plsBtn); lv_label_set_text(i, LV_SYMBOL_PLUS);
          lv_obj_set_style_text_font(i, &lv_font_montserrat_24, 0);
          lv_obj_set_style_text_color(i, lv_color_white(), 0); lv_obj_center(i); }
    }

    // ── Manual panel: large [−] value [+] ───────────────────────────────────
    manualPanel_ = lv_obj_create(screen_);
    lv_obj_set_size(manualPanel_, 480, 210);
    lv_obj_set_pos(manualPanel_, 0, 148);
    lv_obj_set_style_bg_opa(manualPanel_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(manualPanel_, 0, 0);
    lv_obj_set_style_pad_all(manualPanel_, 0, 0);
    lv_obj_remove_flag(manualPanel_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *minLrg = lv_button_create(manualPanel_);
    lv_obj_set_size(minLrg, 100, 85);
    lv_obj_align(minLrg, LV_ALIGN_CENTER, -140, 0);
    lv_obj_set_style_bg_color(minLrg, CLR_BTN, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(minLrg, CLR_BTN_PR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(minLrg, 16, 0);
    lv_obj_set_style_shadow_width(minLrg, 0, 0);
    lv_obj_add_event_cb(minLrg, onMinus, LV_EVENT_CLICKED, (void *)(intptr_t)3);
    { lv_obj_t *i = lv_label_create(minLrg); lv_label_set_text(i, LV_SYMBOL_MINUS);
      lv_obj_set_style_text_font(i, &lv_font_montserrat_24, 0);
      lv_obj_set_style_text_color(i, lv_color_white(), 0); lv_obj_center(i); }

    valueLbls_[3] = lv_label_create(manualPanel_);
    lv_obj_set_style_text_font(valueLbls_[3], &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(valueLbls_[3], lv_color_white(), 0);
    lv_obj_align(valueLbls_[3], LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *plsLrg = lv_button_create(manualPanel_);
    lv_obj_set_size(plsLrg, 100, 85);
    lv_obj_align(plsLrg, LV_ALIGN_CENTER, 140, 0);
    lv_obj_set_style_bg_color(plsLrg, CLR_BTN, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(plsLrg, CLR_BTN_PR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(plsLrg, 16, 0);
    lv_obj_set_style_shadow_width(plsLrg, 0, 0);
    lv_obj_add_event_cb(plsLrg, onPlus, LV_EVENT_CLICKED, (void *)(intptr_t)3);
    { lv_obj_t *i = lv_label_create(plsLrg); lv_label_set_text(i, LV_SYMBOL_PLUS);
      lv_obj_set_style_text_font(i, &lv_font_montserrat_24, 0);
      lv_obj_set_style_text_color(i, lv_color_white(), 0); lv_obj_center(i); }

    // ── Back button ──────────────────────────────────────────────────────────
    lv_obj_t *backBtn = lv_button_create(screen_);
    lv_obj_set_size(backBtn, 200, 55);
    lv_obj_align(backBtn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(backBtn, CLR_BACK, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(backBtn, lv_color_hex(0x1C2A30), LV_STATE_PRESSED);
    lv_obj_set_style_radius(backBtn, 14, 0);
    lv_obj_set_style_shadow_width(backBtn, 0, 0);
    lv_obj_add_event_cb(backBtn, onBack, LV_EVENT_CLICKED, this);
    { lv_obj_t *l = lv_label_create(backBtn);
      lv_label_set_text(l, LV_SYMBOL_LEFT " Retour");
      lv_obj_set_style_text_font(l, &lv_font_montserrat_24, 0);
      lv_obj_set_style_text_color(l, lv_color_white(), 0); lv_obj_center(l); }

    updateMode();
    updateLabels();
}

void BrightnessScreen::refresh() {
    updateMode();
    updateLabels();
}

void BrightnessScreen::updateMode() {
    bool isAuto = ui_->brightness().autoMode;
    lv_obj_set_style_bg_color(autoBtn_,   isAuto ? CLR_BTN_ACT : CLR_BTN, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(manualBtn_, isAuto ? CLR_BTN : CLR_BTN_ACT, LV_STATE_DEFAULT);
    if (isAuto) {
        lv_obj_remove_flag(autoPanel_,  LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(manualPanel_,   LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(autoPanel_,     LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(manualPanel_,LV_OBJ_FLAG_HIDDEN);
    }
}

void BrightnessScreen::updateLabels() {
    auto &b = ui_->brightness();
    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", b.dayLevel);     lv_label_set_text(valueLbls_[0], buf);
    snprintf(buf, sizeof(buf), "%d%%", b.eveningLevel); lv_label_set_text(valueLbls_[1], buf);
    snprintf(buf, sizeof(buf), "%d%%", b.nightLevel);   lv_label_set_text(valueLbls_[2], buf);
    snprintf(buf, sizeof(buf), "%d%%", b.manualLevel);  lv_label_set_text(valueLbls_[3], buf);
}

void BrightnessScreen::adjustLevel(int field, int delta) {
    auto &b = ui_->brightness();
    uint8_t *lv = nullptr;
    switch (field) {
        case 0: lv = &b.dayLevel;     break;
        case 1: lv = &b.eveningLevel; break;
        case 2: lv = &b.nightLevel;   break;
        case 3: lv = &b.manualLevel;  break;
        default: return;
    }
    int v = (int)*lv + delta;
    if (v < 10)  v = 10;
    if (v > 100) v = 100;
    *lv = (uint8_t)v;
    ui_->saveBrightness();
    updateLabels();
    Lcd_SetBacklight(ui_->getTargetBrightness());
}

// ─── Callbacks ──────────────────────────────────────────────────────────────

void BrightnessScreen::onAutoToggle(lv_event_t *e) {
    auto *self = static_cast<BrightnessScreen *>(lv_event_get_user_data(e));
    self->ui_->brightness().autoMode = true;
    self->ui_->saveBrightness();
    self->updateMode();
    Lcd_SetBacklight(self->ui_->getTargetBrightness());
}

void BrightnessScreen::onManualToggle(lv_event_t *e) {
    auto *self = static_cast<BrightnessScreen *>(lv_event_get_user_data(e));
    self->ui_->brightness().autoMode = false;
    self->ui_->saveBrightness();
    self->updateMode();
    Lcd_SetBacklight(self->ui_->getTargetBrightness());
}

void BrightnessScreen::onMinus(lv_event_t *e) {
    auto *self = selfFromEvent(e);
    if (!self) return;
    self->adjustLevel((int)(intptr_t)lv_event_get_user_data(e), -10);
}

void BrightnessScreen::onPlus(lv_event_t *e) {
    auto *self = selfFromEvent(e);
    if (!self) return;
    self->adjustLevel((int)(intptr_t)lv_event_get_user_data(e), +10);
}

void BrightnessScreen::onBack(lv_event_t *e) {
    auto *self = static_cast<BrightnessScreen *>(lv_event_get_user_data(e));
    self->ui_->showAdminScreen();
}
