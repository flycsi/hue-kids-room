#include "HueClient.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Party mode: 8 vivid colors cycling through the rainbow
static const HueColor PARTY_COLORS[] = {
    {0,     254, 220, true},  // Red
    {7281,  254, 220, true},  // Orange
    {12750, 254, 220, true},  // Yellow
    {25500, 254, 220, true},  // Green
    {43690, 254, 220, true},  // Blue
    {46920, 254, 220, true},  // Indigo
    {56100, 254, 220, true},  // Violet
    {0,     200, 254, true},  // Pink
};
static const int PARTY_STEPS = sizeof(PARTY_COLORS) / sizeof(PARTY_COLORS[0]);

HueClient::HueClient(const char *bridgeIp, const char *username)
    : bridgeIp_(bridgeIp), username_(username) {}

HueClient::~HueClient() {}

void HueClient::sendState(int lightId, const char *jsonBody) {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    String url = String("http://") + bridgeIp_ + "/api/" + username_
                 + "/lights/" + lightId + "/state";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.PUT(jsonBody);
    http.end();
}

void HueClient::broadcastColor(const HueColor &c) {
    JsonDocument doc;
    doc["on"]  = c.on;
    doc["hue"] = c.hue;
    doc["sat"] = c.sat;
    doc["bri"] = c.bri;
    doc["transitiontime"] = 2;  // 200 ms
    String body;
    serializeJson(doc, body);

    for (int i = 0; i < HUE_LIGHT_COUNT; i++) {
        sendState(HUE_LIGHT_IDS[i], body.c_str());
    }
}

void HueClient::broadcastCt(uint16_t ct, uint8_t bri) {
    JsonDocument doc;
    doc["on"]  = true;
    doc["ct"]  = ct;
    doc["bri"] = bri;
    doc["transitiontime"] = 10;  // 1 s
    String body;
    serializeJson(doc, body);

    for (int i = 0; i < HUE_LIGHT_COUNT; i++) {
        sendState(HUE_LIGHT_IDS[i], body.c_str());
    }
}

void HueClient::applyColor(const HueColor &color) {
    color_      = color;
    lightsOn_   = color.on;
    mode_       = AppMode::Normal;
    partyActive_= false;
    broadcastColor(color_);
}

void HueClient::toggleLights() {
    lightsOn_ = !lightsOn_;
    partyActive_ = false;
    mode_ = AppMode::Normal;

    JsonDocument doc;
    doc["on"] = lightsOn_;
    String body;
    serializeJson(doc, body);

    for (int i = 0; i < HUE_LIGHT_COUNT; i++) {
        sendState(HUE_LIGHT_IDS[i], body.c_str());
    }
}

void HueClient::setNightMode() {
    mode_        = AppMode::Night;
    partyActive_ = false;
    lightsOn_    = true;
    broadcastCt(HUE_NIGHT_CT, HUE_NIGHT_BRI);
}

void HueClient::setPartyMode(bool enable) {
    if (enable) {
        mode_        = AppMode::Party;
        partyActive_ = true;
        lightsOn_    = true;
        partyStep_   = 0;
        lastPartyMs_ = 0;  // force immediate first update
    } else {
        mode_        = AppMode::Normal;
        partyActive_ = false;
        broadcastColor(color_);
    }
}

void HueClient::tick() {
    if (!partyActive_) return;

    uint32_t now = millis();
    if (now - lastPartyMs_ >= HUE_PARTY_INTERVAL_MS) {
        lastPartyMs_ = now;
        broadcastColor(PARTY_COLORS[partyStep_]);
        partyStep_ = (partyStep_ + 1) % PARTY_STEPS;
    }
}
