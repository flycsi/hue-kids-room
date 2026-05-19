#pragma once

#include "lvgl.h"
#include "../hue/HueClient.h"

class AppUI;

class ColorScreen {
public:
    explicit ColorScreen(AppUI *ui);

    lv_obj_t *screen() { return screen_; }

    /** Initialise the wheel with the current color before showing */
    void setInitialColor(HueColor color);

private:
    AppUI     *ui_;
    lv_obj_t  *screen_     = nullptr;
    lv_obj_t  *hueSlider_  = nullptr;
    lv_obj_t  *briSlider_  = nullptr;

    void build();

    static void onColorChanged(lv_event_t *e);
    static void onBriChanged(lv_event_t *e);
    static void onBackBtn(lv_event_t *e);
};
