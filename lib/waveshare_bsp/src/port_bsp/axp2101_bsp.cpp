#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "axp2101_bsp.h"

#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h"

static const char *TAG = "axp2101";

static XPowersPMU    axp2101;
static I2cMasterBus *s_i2cbus   = nullptr;
static i2c_master_dev_handle_t s_devHandle = nullptr;
static uint8_t       s_devAddr;

static int pmic_read(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len) {
    for (int i = 0; i < 3; i++) {
        if (s_i2cbus->i2c_read_buff(s_devHandle, regAddr, data, len) == ESP_OK) return 0;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return -1;
}

static int pmic_write(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len) {
    for (int i = 0; i < 3; i++) {
        if (s_i2cbus->i2c_write_buff(s_devHandle, regAddr, data, len) == ESP_OK) return 0;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return -1;
}

void Custom_PmicPortInit(I2cMasterBus *i2cbus, uint8_t dev_addr) {
    if (!s_i2cbus)    s_i2cbus  = i2cbus;
    if (!s_devHandle) {
        i2c_device_config_t dev_cfg = {};
        dev_cfg.dev_addr_length  = I2C_ADDR_BIT_LEN_7;
        dev_cfg.scl_speed_hz     = 100000;
        dev_cfg.device_address   = dev_addr;
        ESP_ERROR_CHECK(i2c_master_bus_add_device(s_i2cbus->Get_I2cBusHandle(), &dev_cfg, &s_devHandle));
        s_devAddr = dev_addr;
    }
    if (axp2101.begin(s_devAddr, pmic_read, pmic_write)) {
        ESP_LOGI(TAG, "AXP2101 init OK");
    } else {
        ESP_LOGE(TAG, "AXP2101 init FAILED");
    }
    Custom_PmicRegisterInit();
}

void Custom_PmicRegisterInit(void) {
    axp2101.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_2000MA);
    if (axp2101.getDC1Voltage()  != 3300) axp2101.setDC1Voltage(3300);
    if (axp2101.getALDO1Voltage()!= 3300) axp2101.setALDO1Voltage(3300);
    if (axp2101.getALDO2Voltage()!= 3300) axp2101.setALDO2Voltage(3300);
    if (axp2101.getALDO3Voltage()!= 3300) axp2101.setALDO3Voltage(3300);
    if (axp2101.getALDO4Voltage()!= 3300) axp2101.setALDO4Voltage(3300);
    axp2101.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_50MA);
    axp2101.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_500MA);
    axp2101.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_50MA);
}

void Axp2101_SetAldo3(uint8_t enable) {
    if (enable) axp2101.enableALDO3(); else axp2101.disableALDO3();
}

void Axp2101_SetAldo2(uint8_t enable) {
    if (enable) axp2101.enableALDO2(); else axp2101.disableALDO2();
}
