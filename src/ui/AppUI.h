#pragma once

#include "lvgl.h"
#include "../hue/HueClient.h"

class HomeScreen;
class ColorScreen;
class PinScreen;
class AdminScreen;

class AppUI {
public:
    explicit AppUI(HueClient *hue);
    ~AppUI();

    void init();

    void showColorScreen();
    void showHomeScreen();
    void showPinScreen();
    void showAdminScreen();

    /** Display a status message on the home screen */
    void setStatus(const char *msg);

    // Called by ColorScreen when the user picks a new color
    void onColorPicked(uint16_t hue, uint8_t sat, uint8_t bri);

    // Called by HomeScreen buttons
    void onNightMode();
    void onPartyMode();
    void onPowerToggle();
    void onSleepMode();

    HueClient *hue() { return hue_; }

private:
    HueClient   *hue_;
    HomeScreen  *homeScreen_  = nullptr;
    ColorScreen *colorScreen_ = nullptr;
    PinScreen   *pinScreen_   = nullptr;
    AdminScreen *adminScreen_ = nullptr;
    lv_timer_t  *statusTimer_ = nullptr;

    static void onStatusUpdate(lv_timer_t *t);
};
