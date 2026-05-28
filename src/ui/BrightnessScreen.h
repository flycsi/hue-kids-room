#pragma once

#include "lvgl.h"

class AppUI;

class BrightnessScreen {
public:
    explicit BrightnessScreen(AppUI *ui);

    lv_obj_t *screen() { return screen_; }
    void refresh();

private:
    AppUI    *ui_;
    lv_obj_t *screen_      = nullptr;
    lv_obj_t *autoBtn_     = nullptr;
    lv_obj_t *manualBtn_   = nullptr;
    lv_obj_t *autoPanel_   = nullptr;
    lv_obj_t *manualPanel_ = nullptr;
    lv_obj_t *valueLbls_[4]= {};  // 0=Jour, 1=Soir, 2=Nuit, 3=Manuel

    void build();
    void updateMode();
    void updateLabels();
    void adjustLevel(int field, int delta);

    static void onAutoToggle(lv_event_t *e);
    static void onManualToggle(lv_event_t *e);
    static void onMinus(lv_event_t *e);
    static void onPlus(lv_event_t *e);
    static void onBack(lv_event_t *e);
};
