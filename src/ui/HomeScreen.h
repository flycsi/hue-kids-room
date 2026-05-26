#pragma once

#include "lvgl.h"
#include "../hue/HueClient.h"

class AppUI;

class HomeScreen {
public:
    explicit HomeScreen(AppUI *ui);

    lv_obj_t *screen() { return screen_; }

    /** Refresh the color preview circle to match current Hue state */
    void updateColorPreview(HueColor color, AppMode mode);

    /** Flash a status message (WiFi connecting, etc.) */
    void setStatus(const char *msg);

    void updateTime(int hour, int minute);
    void updateBattery(int pct, bool charging);

private:
    AppUI     *ui_;
    lv_obj_t  *screen_     = nullptr;
    lv_obj_t  *colorCircle_= nullptr;
    lv_obj_t  *statusLabel_= nullptr;
    lv_obj_t  *timeLabel_  = nullptr;
    lv_obj_t  *battLabel_  = nullptr;
    lv_obj_t  *nightBtn_   = nullptr;
    lv_obj_t  *partyBtn_   = nullptr;
    lv_obj_t  *powerBtn_   = nullptr;
    lv_obj_t  *sleepBtn_   = nullptr;

    void build();
    void styleBtn(lv_obj_t *btn, lv_color_t color);

    static void onColorCircleTap(lv_event_t *e);
    static void onNightBtn(lv_event_t *e);
    static void onPartyBtn(lv_event_t *e);
    static void onPowerBtn(lv_event_t *e);
    static void onSleepBtn(lv_event_t *e);
    static void onGesture(lv_event_t *e);
};
