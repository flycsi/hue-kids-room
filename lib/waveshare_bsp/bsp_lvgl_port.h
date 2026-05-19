#pragma once

#include "src/port_bsp/i2c_bsp.h"

bool bsp_lvgl_lock(int timeout_ms);
void bsp_lvgl_unlock(void);
void bsp_lvgl_init(I2cMasterBus &i2cbus);

/** Set display backlight brightness 0-100 */
void Lcd_SetBacklight(uint8_t brightness);
