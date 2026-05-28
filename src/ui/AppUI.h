#pragma once

#include "lvgl.h"
#include "../hue/HueClient.h"
#include "config.h"

class HomeScreen;
class ColorScreen;
class PinScreen;
class AdminScreen;
class BrightnessScreen;
class RoomScreen;
class ScenesScreen;

struct BrightnessSettings {
    bool    autoMode     = BRIGHTNESS_AUTO_DEFAULT;
    uint8_t manualLevel  = BRIGHTNESS_MANUAL_DEFAULT;
    uint8_t dayLevel     = BRIGHTNESS_DAY_DEFAULT;
    uint8_t eveningLevel = BRIGHTNESS_EVE_DEFAULT;
    uint8_t nightLevel   = BRIGHTNESS_NIGHT_DEFAULT;
};

class AppUI {
public:
    explicit AppUI(HueClient *hue);
    ~AppUI();

    void init();

    void showColorScreen();
    void showHomeScreen();
    void showPinScreen();
    void showAdminScreen();
    void showBrightnessScreen();
    void showRoomScreen();
    void showScenesScreen();

    /** Display a status message on the home screen */
    void setStatus(const char *msg);

    // Called by ColorScreen when the user picks a new color
    void onColorPicked(uint16_t hue, uint8_t sat, uint8_t bri);

    // Called by HomeScreen buttons
    void onPartyMode();
    void onPowerToggle();
    void onSleepMode();

    HueClient *hue() { return hue_; }

    // Brightness management
    BrightnessSettings& brightness() { return brightness_; }
    uint8_t  getTargetBrightness() const;
    void     saveBrightness();
    void     setScreenOn(bool on);

private:
    HueClient        *hue_;
    HomeScreen       *homeScreen_       = nullptr;
    ColorScreen      *colorScreen_      = nullptr;
    PinScreen        *pinScreen_        = nullptr;
    AdminScreen      *adminScreen_      = nullptr;
    BrightnessScreen *brightnessScreen_ = nullptr;
    RoomScreen       *roomScreen_       = nullptr;
    ScenesScreen     *scenesScreen_     = nullptr;
    lv_timer_t       *statusTimer_      = nullptr;

    BrightnessSettings brightness_;
    bool               screenIsOn_     = true;

    void loadBrightness();

    static void onStatusUpdate(lv_timer_t *t);
};
