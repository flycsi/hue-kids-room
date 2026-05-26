#pragma once

#include "lvgl.h"
#include "../hue/HueClient.h"

class HomeScreen;
class ColorScreen;

class AppUI {
public:
    explicit AppUI(HueClient *hue);
    ~AppUI();

    void init();

    void showColorScreen();
    void showHomeScreen();

    /** Display a status message on the home screen */
    void setStatus(const char *msg);

    // Called by ColorScreen when the user picks a new color
    void onColorPicked(uint16_t hue, uint8_t sat, uint8_t bri);

    // Called by HomeScreen buttons
    void onNightMode();
    void onPartyMode();
    void onPowerToggle();

    HueClient *hue() { return hue_; }

private:
    HueClient   *hue_;
    HomeScreen  *homeScreen_  = nullptr;
    ColorScreen *colorScreen_ = nullptr;
    lv_timer_t  *statusTimer_ = nullptr;

    static void onStatusUpdate(lv_timer_t *t);
};
