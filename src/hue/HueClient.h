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

struct HueGroup {
    int  id;
    char name[32];
};

struct HueScene {
    char id[16];
    char name[32];
};

class HueClient {
public:
    HueClient(const char *bridgeIp, const char *username);
    ~HueClient();

    /** Apply a color to all lights in the active group */
    void applyColor(const HueColor &color);

    /** Toggle all lights on/off */
    void toggleLights();

    /** Activate night mode (warm dim) */
    void setNightMode();

    /** Start/stop party mode (color cycling) */
    void setPartyMode(bool enable);

    /** Start sleep timer: warm orange → dims to 5% over configured duration */
    void setSleepMode();

    /** Sleep timer duration (stored in NVS) */
    uint32_t getSleepDurationMs() const { return sleepDurationMs_; }
    void setSleepDuration(uint32_t ms);

    bool isPartyMode()  const { return partyActive_; }
    bool isSleepMode()  const { return mode_ == AppMode::Sleep; }
    bool isLightsOn()   const { return lightsOn_; }
    AppMode getMode()   const { return mode_; }

    /** Remaining sleep time in ms (0 if not in sleep mode) */
    uint32_t sleepRemainingMs() const;

    HueColor currentColor() const { return color_; }

    /** Fetch all rooms/zones from the bridge (blocking HTTP — call from background task) */
    int fetchGroups(HueGroup *out, int maxCount);

    /** Fetch GroupScene scenes for a given group (blocking HTTP — call from background task) */
    int fetchScenes(HueScene *out, int maxCount, int groupId);

    /** Activate a scene on the active group */
    void applyScene(const char *sceneId);

    /** Persist the selected room (stored in NVS) */
    void setActiveGroup(int id, const char *name);
    int         getActiveGroupId()   const { return activeGroupId_; }
    const char *getActiveGroupName() const { return activeGroupName_; }

    /** Call from main loop — drives party color cycle and sleep dimming */
    void tick();

private:
    const char *bridgeIp_;
    const char *username_;

    HueColor color_;
    AppMode  mode_        = AppMode::Normal;
    bool     lightsOn_    = true;
    bool     partyActive_ = false;

    uint32_t lastPartyMs_     = 0;
    uint8_t  partyStep_       = 0;

    uint32_t sleepStartMs_    = 0;
    uint32_t lastSleepStepMs_ = 0;
    uint32_t sleepDurationMs_ = 15UL * 60 * 1000;

    int  activeGroupId_      = 0;
    char activeGroupName_[32]= "Ma Chambre";

    static constexpr uint8_t SLEEP_START_BRI = 200;
    static constexpr uint8_t SLEEP_END_BRI   = 13;  // ~5% of 254

    /** Send a JSON body to the active group action endpoint (or individual lights fallback) */
    void sendGroupAction(const char *jsonBody);

    /** Fallback: send to a single light by ID */
    void sendState(int lightId, const char *jsonBody);

    void broadcastColor(const HueColor &c);
    void broadcastCt(uint16_t ct, uint8_t bri);
};
