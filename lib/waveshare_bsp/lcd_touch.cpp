#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include "lcd_touch.h"
#include "esp_log.h"

static const char *TAG = "lcd_touch";

LcdTouchPanel::LcdTouchPanel(I2cMasterBus &i2cbus, int dev_addr,
                              int touch_rst_pin, int touch_int_pin)
    : i2cbus_(i2cbus)
{
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length  = I2C_ADDR_BIT_LEN_7;
    dev_cfg.scl_speed_hz     = 400000;
    dev_cfg.device_address   = (uint16_t)dev_addr;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2cbus_.Get_I2cBusHandle(), &dev_cfg, &touch_dev_handle));

    if (touch_rst_pin != GPIO_NUM_NC) {
        gpio_config_t io_conf = {};
        io_conf.intr_type    = GPIO_INTR_DISABLE;
        io_conf.mode         = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = 1ULL << touch_rst_pin;
        io_conf.pull_up_en   = GPIO_PULLUP_ENABLE;
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&io_conf));
        touch_rst_pin_ = touch_rst_pin;
        ResetTouch();
    }
}

LcdTouchPanel::~LcdTouchPanel() {}

void LcdTouchPanel::ResetTouch() {
    if (touch_rst_pin_ == GPIO_NUM_NC) return;
    gpio_set_level((gpio_num_t)touch_rst_pin_, 1); vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level((gpio_num_t)touch_rst_pin_, 0); vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level((gpio_num_t)touch_rst_pin_, 1); vTaskDelay(pdMS_TO_TICKS(200));
}

uint8_t LcdTouchPanel::GetCoords(uint16_t *x, uint16_t *y) {
    uint8_t reg[2]  = {0xD0, 0x00};
    uint8_t data[10] = {};
    i2cbus_.i2c_master_write_read_dev(touch_dev_handle, reg, 2, data, 10);

    if (data[6] != 0xAB) return 0;
    if ((data[5] & 0x7F) == 0) return 0;
    if ((data[0] & 0x0F) != 0x06) return 0;

    *y = ((uint16_t)data[1] << 4) | (data[3] >> 4);
    // Mirror X axis (display is right-to-left in the raw touch data)
    *x = BSP_LCD_H_RES - (((uint16_t)data[2] << 4) | (data[3] & 0x0F));
    return 1;
}
