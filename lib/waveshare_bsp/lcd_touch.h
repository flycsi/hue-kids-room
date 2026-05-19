#pragma once

#include "src/port_bsp/i2c_bsp.h"
#include "driver/gpio.h"
#include "user_config.h"

class LcdTouchPanel {
private:
    I2cMasterBus              &i2cbus_;
    i2c_master_dev_handle_t    touch_dev_handle = nullptr;
    int                        touch_rst_pin_   = GPIO_NUM_NC;

public:
    LcdTouchPanel(I2cMasterBus &i2cbus, int dev_addr,
                  int touch_rst_pin = BSP_LCD_TOUCH_RST,
                  int touch_int_pin = GPIO_NUM_NC);
    ~LcdTouchPanel();

    /** Returns true and fills x/y if a touch point is active */
    uint8_t GetCoords(uint16_t *x, uint16_t *y);
    void    ResetTouch();
};
