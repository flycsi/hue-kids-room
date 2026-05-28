#pragma once

#include "lvgl.h"
#include "../hue/HueClient.h"

class AppUI;

class ColorScreen {
public:
    explicit ColorScreen(AppUI *ui);

    lv_obj_t *screen() { return screen_; }

    /** Set initial color before showing the screen */
    void setInitialColor(HueColor color);

private:
    AppUI     *ui_;
    lv_obj_t  *screen_      = nullptr;
    lv_obj_t  *previewCirc_ = nullptr;
    lv_obj_t  *cursor_      = nullptr;
    lv_obj_t  *briSlider_   = nullptr;
    lv_obj_t  *touchZone_   = nullptr;

    uint16_t  currentHue_ = 8192;  // ~45° warm default
    uint8_t   currentBri_ = 200;

    void build();
    void updatePreview();
    void moveCursor(uint16_t hue16);

    static void onTouchZone(lv_event_t *e);
    static void onBriChanged(lv_event_t *e);
    static void onBackBtn(lv_event_t *e);
};
