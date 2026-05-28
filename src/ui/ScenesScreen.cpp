#include "ScenesScreen.h"
#include "AppUI.h"
#include "ui_utils.h"
#include "bsp_lvgl_port.h"
#include <freertos/task.h>

#define CLR_BG      lv_color_hex(0x0D1B2A)
#define CLR_BTN     lv_color_hex(0x1E3A5F)
#define CLR_BTN_PR  lv_color_hex(0x0D1B2A)
#define CLR_CANCEL  lv_color_hex(0x37474F)

// ─── Background fetch task ───────────────────────────────────────────────────

struct ScenesFetchArgs { HueClient *hue; ScenesScreen *screen; int groupId; };

static void sceneFetchTask(void *arg) {
    auto *fa = static_cast<ScenesFetchArgs *>(arg);
    HueScene scenes[ScenesScreen::MAX_SCENES];
    int count = fa->hue->fetchScenes(scenes, ScenesScreen::MAX_SCENES, fa->groupId);

    if (bsp_lvgl_lock(2000)) {
        if (count > 0) fa->screen->onFetchDone(scenes, count);
        else           fa->screen->onFetchError();
        bsp_lvgl_unlock();
    }
    delete fa;
    vTaskDelete(nullptr);
}

// ─── Construction ─────────────────────────────────────────────────────────────

ScenesScreen::ScenesScreen(AppUI *ui) : ui_(ui) { build(); }

void ScenesScreen::build() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_user_data(screen_, this);
    lv_obj_remove_flag(screen_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen_, CLR_BG, 0);
    lv_obj_set_style_bg_opa(screen_, LV_OPA_COVER, 0);

    // Title
    lv_obj_t *title = lv_label_create(screen_);
    lv_label_set_text(title, "Choisir une scene");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Status label (loading / error)
    statusLbl_ = lv_label_create(screen_);
    lv_label_set_text(statusLbl_, "Chargement...");
    lv_obj_set_style_text_font(statusLbl_, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(statusLbl_, lv_color_make(100, 160, 220), 0);
    lv_obj_align(statusLbl_, LV_ALIGN_CENTER, 0, -30);

    // Scrollable scene list (hidden until populated)
    list_ = lv_obj_create(screen_);
    lv_obj_set_size(list_, 440, 320);
    lv_obj_set_pos(list_, 20, 70);
    lv_obj_set_style_bg_opa(list_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list_, 0, 0);
    lv_obj_set_style_pad_all(list_, 0, 0);
    lv_obj_add_flag(list_, LV_OBJ_FLAG_HIDDEN);

    // Cancel button
    lv_obj_t *cancelBtn = lv_button_create(screen_);
    lv_obj_set_size(cancelBtn, 200, 55);
    lv_obj_align(cancelBtn, LV_ALIGN_BOTTOM_MID, 0, -18);
    lv_obj_set_style_bg_color(cancelBtn, CLR_CANCEL, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cancelBtn, lv_color_hex(0x1C2A30), LV_STATE_PRESSED);
    lv_obj_set_style_radius(cancelBtn, 14, 0);
    lv_obj_set_style_shadow_width(cancelBtn, 0, 0);
    lv_obj_add_event_cb(cancelBtn, onCancel, LV_EVENT_CLICKED, this);
    {
        lv_obj_t *lbl = lv_label_create(cancelBtn);
        lv_label_set_text(lbl, LV_SYMBOL_CLOSE "  Annuler");
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_center(lbl);
    }
}

// ─── Public methods ───────────────────────────────────────────────────────────

void ScenesScreen::startFetch() {
    showLoading();
    clearList();
    auto *fa = new ScenesFetchArgs{ ui_->hue(), this, ui_->hue()->getActiveGroupId() };
    xTaskCreate(sceneFetchTask, "hue_scenes", 6144, fa, 1, nullptr);
}

void ScenesScreen::showLoading() {
    lv_label_set_text(statusLbl_, "Chargement...");
    lv_obj_remove_flag(statusLbl_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(list_, LV_OBJ_FLAG_HIDDEN);
}

void ScenesScreen::clearList() {
    lv_obj_clean(list_);
    entryCount_ = 0;
}

void ScenesScreen::onFetchDone(HueScene *scenes, int count) {
    lv_obj_add_flag(statusLbl_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(list_, LV_OBJ_FLAG_HIDDEN);

    entryCount_ = count;

    const lv_coord_t BTN_H = 68, GAP = 10;

    for (int i = 0; i < count; i++) {
        strlcpy(entries_[i].id, scenes[i].id, sizeof(entries_[i].id));
        stripAccents(entries_[i].name, scenes[i].name, sizeof(entries_[i].name));

        lv_obj_t *btn = lv_button_create(list_);
        lv_obj_set_size(btn, 440, BTN_H);
        lv_obj_set_pos(btn, 0, i * (BTN_H + GAP));
        lv_obj_set_style_bg_color(btn, CLR_BTN, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(btn, CLR_BTN_PR, LV_STATE_PRESSED);
        lv_obj_set_style_radius(btn, 16, 0);
        lv_obj_set_style_shadow_width(btn, 0, 0);
        lv_obj_add_event_cb(btn, onSceneTap, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        lv_obj_t *nameLbl = lv_label_create(btn);
        lv_obj_set_style_text_font(nameLbl, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(nameLbl, lv_color_white(), 0);
        lv_obj_align(nameLbl, LV_ALIGN_LEFT_MID, 20, 0);
        lv_label_set_text(nameLbl, entries_[i].name);
    }
}

void ScenesScreen::onFetchError() {
    lv_label_set_text(statusLbl_, "Erreur — WiFi connecte ?");
    lv_obj_remove_flag(statusLbl_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(list_, LV_OBJ_FLAG_HIDDEN);
}

// ─── Callbacks ────────────────────────────────────────────────────────────────

void ScenesScreen::onSceneTap(lv_event_t *e) {
    lv_obj_t *btn = static_cast<lv_obj_t *>(lv_event_get_target(e));
    auto *self = static_cast<ScenesScreen *>(lv_obj_get_user_data(lv_obj_get_screen(btn)));
    if (!self) return;
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    if (idx < 0 || idx >= self->entryCount_) return;

    self->ui_->hue()->applyScene(self->entries_[idx].id);
    self->ui_->setStatus(self->entries_[idx].name);
    self->ui_->showHomeScreen();
}

void ScenesScreen::onCancel(lv_event_t *e) {
    auto *self = static_cast<ScenesScreen *>(lv_event_get_user_data(e));
    self->ui_->showHomeScreen();
}
