#include <freertos/FreeRTOS.h>
#include "i2c_bsp.h"

static const uint32_t I2C_DATA_TICKS = pdMS_TO_TICKS(5000);
static const uint32_t I2C_DONE_TICKS = pdMS_TO_TICKS(1000);

I2cMasterBus::I2cMasterBus(int scl_pin, int sda_pin, int i2c_port) {
    i2c_master_bus_config_t cfg = {};
    cfg.clk_source               = I2C_CLK_SRC_DEFAULT;
    cfg.i2c_port                 = (i2c_port_t)i2c_port;
    cfg.scl_io_num               = (gpio_num_t)scl_pin;
    cfg.sda_io_num               = (gpio_num_t)sda_pin;
    cfg.glitch_ignore_cnt        = 7;
    cfg.flags.enable_internal_pullup = true;
    ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &user_i2c_handle));
}

I2cMasterBus::~I2cMasterBus() {}

int I2cMasterBus::i2c_write_buff(i2c_master_dev_handle_t dev_handle, int reg, uint8_t *buf, uint8_t len) {
    i2c_master_bus_wait_all_done(user_i2c_handle, I2C_DONE_TICKS);
    if (reg == -1) {
        return i2c_master_transmit(dev_handle, buf, len, I2C_DATA_TICKS);
    }
    uint8_t *pbuf = (uint8_t *)malloc(len + 1);
    pbuf[0] = (uint8_t)reg;
    for (uint8_t i = 0; i < len; i++) pbuf[i + 1] = buf[i];
    int ret = i2c_master_transmit(dev_handle, pbuf, len + 1, I2C_DATA_TICKS);
    free(pbuf);
    return ret;
}

int I2cMasterBus::i2c_master_write_read_dev(i2c_master_dev_handle_t dev_handle,
                                             uint8_t *writeBuf, uint8_t writeLen,
                                             uint8_t *readBuf,  uint8_t readLen) {
    i2c_master_bus_wait_all_done(user_i2c_handle, I2C_DONE_TICKS);
    return i2c_master_transmit_receive(dev_handle, writeBuf, writeLen, readBuf, readLen, I2C_DATA_TICKS);
}

int I2cMasterBus::i2c_read_buff(i2c_master_dev_handle_t dev_handle, int reg, uint8_t *buf, uint8_t len) {
    i2c_master_bus_wait_all_done(user_i2c_handle, I2C_DONE_TICKS);
    if (reg == -1) {
        return i2c_master_receive(dev_handle, buf, len, I2C_DATA_TICKS);
    }
    uint8_t addr = (uint8_t)reg;
    return i2c_master_transmit_receive(dev_handle, &addr, 1, buf, len, I2C_DATA_TICKS);
}

i2c_master_bus_handle_t I2cMasterBus::Get_I2cBusHandle() {
    return user_i2c_handle;
}
