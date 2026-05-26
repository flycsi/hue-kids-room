#include "AppUI.h"
#include "HomeScreen.h"
#include "ColorScreen.h"
#include <time.h>
#include "axp2101_bsp.h"

AppUI::AppUI(HueClient *hue) : hue_(hue) {}

AppUI::~AppUI() {
    delete homeScreen_;
    delete colorScreen_;
}

void AppUI::init() {
    homeScreen_  = new HomeScreen(this);
    colorScreen_ = new ColorScreen(this);
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
}

void AppUI::showColorScreen() {
    colorScreen_->setInitialColor(hue_->currentColor());
    lv_screen_load_anim(colorScreen_->screen(), LV_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, false);
}

void AppUI::showHomeScreen() {
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

void AppUI::onNightMode() {
    hue_->setNightMode();
    homeScreen_->updateColorPreview(hue_->currentColor(), AppMode::Night);
    homeScreen_->setStatus("Mode Nuit");
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

void AppUI::setStatus(const char *msg) {
    if (homeScreen_) homeScreen_->setStatus(msg);
}
