#pragma once

#include "lvgl.h"

class AppUI;

class AdminScreen {
public:
    explicit AdminScreen(AppUI *ui);

    lv_obj_t *screen() { return screen_; }
    void refresh();

private:
    AppUI    *ui_;
    lv_obj_t *screen_      = nullptr;
    lv_obj_t *durationLbl_ = nullptr;

    void build();
    void updateDurationLabel();

    static void onMinus(lv_event_t *e);
    static void onPlus(lv_event_t *e);
    static void onBrightness(lv_event_t *e);
    static void onBack(lv_event_t *e);
};
