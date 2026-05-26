#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

enum class AppMode { Normal, Night, Party, Sleep };

struct HueColor {
    uint16_t hue = 43690;  // 0-65535  (43690 ≈ blue-purple)
    uint8_t  sat = 200;    // 0-254
    uint8_t  bri = 200;    // 0-254
    bool     on  = true;
};

class HueClient {
public:
    HueClient(const char *bridgeIp, const char *username);
    ~HueClient();

    /** Apply a color to all configured lights */
    void applyColor(const HueColor &color);

    /** Toggle all lights on/off */
    void toggleLights();

    /** Activate night mode (warm dim) */
    void setNightMode();

    /** Start/stop party mode (color cycling) */
    void setPartyMode(bool enable);

    /** Start sleep timer: warm orange → dims to 5% over 15 min */
    void setSleepMode();

    bool isPartyMode()  const { return partyActive_; }
    bool isSleepMode()  const { return mode_ == AppMode::Sleep; }
    bool isLightsOn()   const { return lightsOn_; }
    AppMode getMode()   const { return mode_; }

    /** Remaining sleep time in ms (0 if not in sleep mode) */
    uint32_t sleepRemainingMs() const;

    HueColor currentColor() const { return color_; }

    /** Call from main loop — drives party color cycle */
    void tick();

private:
    const char *bridgeIp_;
    const char *username_;

    HueColor color_;
    AppMode  mode_       = AppMode::Normal;
    bool     lightsOn_   = true;
    bool     partyActive_= false;

    uint32_t lastPartyMs_    = 0;
    uint8_t  partyStep_      = 0;

    uint32_t sleepStartMs_   = 0;
    uint32_t lastSleepStepMs_= 0;

    static constexpr uint32_t SLEEP_DURATION_MS = 15UL * 60 * 1000;
    static constexpr uint8_t  SLEEP_START_BRI   = 200;
    static constexpr uint8_t  SLEEP_END_BRI     = 13;   // ~5 % of 254

    void sendState(int lightId, const char *jsonBody);
    void broadcastColor(const HueColor &c);
    void broadcastCt(uint16_t ct, uint8_t bri);
};
