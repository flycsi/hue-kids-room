#include "AppUI.h"
#include "HomeScreen.h"
#include "ColorScreen.h"
#include "PinScreen.h"
#include "AdminScreen.h"
#include "BrightnessScreen.h"
#include "RoomScreen.h"
#include "ScenesScreen.h"
#include <time.h>
#include <Preferences.h>
#include "axp2101_bsp.h"
#include "bsp_lvgl_port.h"

AppUI::AppUI(HueClient *hue) : hue_(hue) {}

AppUI::~AppUI() {
    delete homeScreen_;
    delete colorScreen_;
    delete pinScreen_;
    delete adminScreen_;
    delete brightnessScreen_;
    delete roomScreen_;
    delete scenesScreen_;
}

void AppUI::loadBrightness() {
    Preferences prefs;
    prefs.begin("huekids", true);
    brightness_.autoMode     = prefs.getBool("briAuto",   BRIGHTNESS_AUTO_DEFAULT);
    brightness_.manualLevel  = prefs.getUChar("briMan",   BRIGHTNESS_MANUAL_DEFAULT);
    brightness_.dayLevel     = prefs.getUChar("briDay",   BRIGHTNESS_DAY_DEFAULT);
    brightness_.eveningLevel = prefs.getUChar("briEve",   BRIGHTNESS_EVE_DEFAULT);
    brightness_.nightLevel   = prefs.getUChar("briNight", BRIGHTNESS_NIGHT_DEFAULT);
    prefs.end();
}

void AppUI::saveBrightness() {
    Preferences prefs;
    prefs.begin("huekids", false);
    prefs.putBool("briAuto",   brightness_.autoMode);
    prefs.putUChar("briMan",   brightness_.manualLevel);
    prefs.putUChar("briDay",   brightness_.dayLevel);
    prefs.putUChar("briEve",   brightness_.eveningLevel);
    prefs.putUChar("briNight", brightness_.nightLevel);
    prefs.end();
}

uint8_t AppUI::getTargetBrightness() const {
    if (!brightness_.autoMode) return brightness_.manualLevel;
    struct tm t = {};
    if (!getLocalTime(&t)) return brightness_.dayLevel;  // NTP pas encore synchro
    int h = t.tm_hour;
    if (h >= BRIGHTNESS_NIGHT_HOUR || h < BRIGHTNESS_DAY_HOUR) return brightness_.nightLevel;
    if (h >= BRIGHTNESS_EVE_HOUR)                               return brightness_.eveningLevel;
    return brightness_.dayLevel;
}

void AppUI::setScreenOn(bool on) {
    screenIsOn_ = on;
    if (on) Lcd_SetBacklight(getTargetBrightness());
}

void AppUI::init() {
    loadBrightness();

    homeScreen_      = new HomeScreen(this);
    colorScreen_     = new ColorScreen(this);
    pinScreen_       = new PinScreen(this);
    adminScreen_     = new AdminScreen(this);
    brightnessScreen_= new BrightnessScreen(this);
    roomScreen_      = new RoomScreen(this);
    scenesScreen_    = new ScenesScreen(this);

    homeScreen_->setTitle(hue_->getActiveGroupName());
    lv_screen_load(homeScreen_->screen());

    // Update status bar every 30 s, and once immediately
    statusTimer_ = lv_timer_create(onStatusUpdate, 30000, this);
    lv_timer_ready(statusTimer_);
}

void AppUI::onStatusUpdate(lv_timer_t *t) {
    auto *self = static_cast<AppUI *>(lv_timer_get_user_data(t));
    if (!self->homeScreen_) return;

    struct tm timeinfo = {};
    if (getLocalTime(&timeinfo)) {
        self->homeScreen_->updateTime(timeinfo.tm_hour, timeinfo.tm_min);
    }

    self->homeScreen_->updateBattery(
        Axp2101_GetBatteryPercent(),
        Axp2101_IsCharging()
    );

    // Apply auto brightness (only when screen is on)
    if (self->screenIsOn_) {
        Lcd_SetBacklight(self->getTargetBrightness());
    }

    // Countdown during sleep mode
    if (self->hue_->isSleepMode()) {
        uint32_t remaining = self->hue_->sleepRemainingMs();
        uint32_t mins = (remaining + 59999) / 60000;  // round up
        char buf[24];
        if (mins > 0) {
            snprintf(buf, sizeof(buf), "Dodo dans %lu min", mins);
        } else {
            snprintf(buf, sizeof(buf), "Bonne nuit !");
        }
        self->homeScreen_->setStatus(buf);
    }
}

void AppUI::showColorScreen() {
    colorScreen_->setInitialColor(hue_->currentColor());
    lv_screen_load_anim(colorScreen_->screen(), LV_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, false);
}

void AppUI::showHomeScreen() {
    homeScreen_->setTitle(hue_->getActiveGroupName());
    homeScreen_->updateColorPreview(hue_->currentColor(), hue_->getMode());
    lv_screen_load_anim(homeScreen_->screen(), LV_SCR_LOAD_ANIM_MOVE_RIGHT, 250, 0, false);
}

void AppUI::onColorPicked(uint16_t hue, uint8_t sat, uint8_t bri) {
    HueColor c;
    c.hue = hue;
    c.sat = sat;
    c.bri = bri;
    c.on  = true;
    hue_->applyColor(c);
    homeScreen_->updateColorPreview(c, AppMode::Normal);
}

void AppUI::onPartyMode() {
    bool startParty = !hue_->isPartyMode();
    hue_->setPartyMode(startParty);
    if (startParty) {
        homeScreen_->setStatus("Mode Fete!");
    } else {
        homeScreen_->setStatus("Mode normal");
        homeScreen_->updateColorPreview(hue_->currentColor(), AppMode::Normal);
    }
}

void AppUI::onPowerToggle() {
    hue_->toggleLights();
    homeScreen_->updateColorPreview(hue_->currentColor(), hue_->getMode());
    homeScreen_->setStatus(hue_->isLightsOn() ? "Allume" : "Eteint");
}

void AppUI::onSleepMode() {
    if (hue_->isSleepMode()) {
        hue_->applyColor(hue_->currentColor());
        homeScreen_->updateColorPreview(hue_->currentColor(), AppMode::Normal);
        homeScreen_->setStatus("Dodo annule");
    } else {
        hue_->setSleepMode();
        homeScreen_->updateColorPreview(hue_->currentColor(), AppMode::Sleep);
        homeScreen_->setStatus("Dodo dans 15 min");
    }
}

void AppUI::setStatus(const char *msg) {
    if (homeScreen_) homeScreen_->setStatus(msg);
}

void AppUI::showPinScreen() {
    pinScreen_->reset();
    lv_screen_load_anim(pinScreen_->screen(), LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 250, 0, false);
}

void AppUI::showAdminScreen() {
    adminScreen_->refresh();
    lv_screen_load_anim(adminScreen_->screen(), LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
}

void AppUI::showBrightnessScreen() {
    brightnessScreen_->refresh();
    lv_screen_load_anim(brightnessScreen_->screen(), LV_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, false);
}

void AppUI::showRoomScreen() {
    roomScreen_->startFetch();
    lv_screen_load_anim(roomScreen_->screen(), LV_SCR_LOAD_ANIM_MOVE_TOP, 250, 0, false);
}

void AppUI::showScenesScreen() {
    scenesScreen_->startFetch();
    lv_screen_load_anim(scenesScreen_->screen(), LV_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, false);
}
