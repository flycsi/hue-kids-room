#include <Arduino.h>
#include <WiFi.h>
#include "bsp_lvgl_port.h"
#include "user_config.h"
#include "i2c_bsp.h"
#include "axp2101_bsp.h"
#include "config.h"
#include "ui/AppUI.h"
#include "hue/HueClient.h"

static I2cMasterBus  i2cBus(BSP_I2C_SCL, BSP_I2C_SDA, I2C_NUM_0);
static HueClient    *hueClient = nullptr;
static AppUI        *appUI     = nullptr;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("[hue-kids-room] start");

    // Power management (AXP2101 PMIC)
    Custom_PmicPortInit(&i2cBus, 0x34);

    // Display + touch + LVGL task
    bsp_lvgl_init(i2cBus);

    // Create app objects
    hueClient = new HueClient(HUE_BRIDGE_IP, HUE_USERNAME);

    // Build UI inside the LVGL lock
    if (bsp_lvgl_lock(0)) {
        appUI = new AppUI(hueClient);
        appUI->init();
        appUI->setStatus("Connexion WiFi...");
        Lcd_SetBacklight(appUI->getTargetBrightness());
        bsp_lvgl_unlock();
    }

    // Connect WiFi in a background task so the UI stays responsive
    xTaskCreate(
        [](void *) {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            Serial.print("[WiFi] connecting");
            for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) {
                Serial.print(".");
                vTaskDelay(pdMS_TO_TICKS(500));
            }

            const bool ok = (WiFi.status() == WL_CONNECTED);
            Serial.println(ok ? "\n[WiFi] OK " + WiFi.localIP().toString()
                              : "\n[WiFi] FAILED");

            if (ok) {
                // CET/CEST (France) — change offset for other timezones
                configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.google.com");
                Serial.println("[NTP] sync started");
            }

            if (appUI && bsp_lvgl_lock(200)) {
                appUI->setStatus(ok ? "Pret !" : "WiFi indisponible");
                bsp_lvgl_unlock();
            }
            vTaskDelete(nullptr);
        },
        "wifi_connect", 4096, nullptr, 1, nullptr);
}

#define SCREEN_TIMEOUT_MS 30000

void loop() {
    if (hueClient) hueClient->tick();

    // Screen timeout: backlight off after 30 s of inactivity, on again on touch
    static bool screenOff = false;
    if (bsp_lvgl_lock(5)) {
        uint32_t idle = lv_display_get_inactive_time(lv_display_get_default());
        bsp_lvgl_unlock();
        if (idle >= SCREEN_TIMEOUT_MS && !screenOff) {
            Lcd_SetBacklight(0);
            screenOff = true;
            if (appUI) appUI->setScreenOn(false);
        } else if (idle < SCREEN_TIMEOUT_MS && screenOff) {
            screenOff = false;
            if (appUI) appUI->setScreenOn(true);   // applique la luminosité gérée
            else Lcd_SetBacklight(100);
        }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
}
