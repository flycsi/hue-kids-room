#include "bsp_lvgl_port.h"
#include "lvgl.h"
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include "src/externLib/esp_lcd_sh8601.h"
#include "user_config.h"
#include "lcd_touch.h"
#include "src/port_bsp/axp2101_bsp.h"

static esp_lcd_panel_handle_t    panel_handle = nullptr;
static esp_lcd_panel_io_handle_t io_handle    = nullptr;
static lv_display_t             *disp         = nullptr;
static lv_indev_t               *indev_drv;
static SemaphoreHandle_t         lvgl_mux     = nullptr;
static LcdTouchPanel            *touch_dev    = nullptr;

#define LCD_BIT_PER_PIXEL  16
#define BYTES_PER_PIXEL    (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
#define BUFF_SIZE          (BSP_LCD_H_RES * 50 * BYTES_PER_PIXEL)

static const sh8601_lcd_init_cmd_t lcd_init_cmds[] = {
    { 0x11, (uint8_t[]){0x00}, 0, 600 },
    { 0xFE, (uint8_t[]){0x20}, 1,   0 },
    { 0x19, (uint8_t[]){0x10}, 1,   0 },
    { 0x1C, (uint8_t[]){0xA0}, 1,   0 },
    { 0xFE, (uint8_t[]){0x00}, 1,   0 },
    { 0xC4, (uint8_t[]){0x80}, 1,   0 },
    { 0x3A, (uint8_t[]){0x55}, 1,   0 },
    { 0x35, (uint8_t[]){0x00}, 1,   0 },
    { 0x36, (uint8_t[]){0x30}, 1,   0 },
    { 0x53, (uint8_t[]){0x20}, 1,   0 },
    { 0x51, (uint8_t[]){0xFF}, 1,   0 },
    { 0x63, (uint8_t[]){0xFF}, 1,   0 },
    { 0x2A, (uint8_t[]){0x00, 0x00, 0x01, 0xDF}, 4, 0 },
    { 0x2B, (uint8_t[]){0x00, 0x00, 0x01, 0xDF}, 4, 0 },
    { 0x29, (uint8_t[]){0x00}, 0, 100 },
};

static void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p) {
    lv_draw_sw_rgb565_swap(color_p, lv_area_get_width(area) * lv_area_get_height(area));
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
    lv_disp_flush_ready(disp);
}

static void my_disp_rounder(lv_event_t *e) {
    lv_area_t *area = (lv_area_t *)lv_event_get_param(e);
    area->x1 = (area->x1 >> 1) << 1;
    area->y1 = (area->y1 >> 1) << 1;
    area->x2 = ((area->x2 >> 1) << 1) + 1;
    area->y2 = ((area->y2 >> 1) << 1) + 1;
}

static void increase_lvgl_tick(void *arg) {
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

bool bsp_lvgl_lock(int timeout_ms) {
    assert(lvgl_mux);
    TickType_t ticks = (timeout_ms == 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(lvgl_mux, ticks) == pdTRUE;
}

void bsp_lvgl_unlock(void) {
    assert(lvgl_mux);
    xSemaphoreGive(lvgl_mux);
}

static void lvgl_port_task(void *arg) {
    uint32_t delay_ms = LVGL_TASK_MAX_DELAY_MS;
    while (1) {
        if (bsp_lvgl_lock(0)) {
            delay_ms = lv_timer_handler();
            bsp_lvgl_unlock();
        }
        if (delay_ms > LVGL_TASK_MAX_DELAY_MS) delay_ms = LVGL_TASK_MAX_DELAY_MS;
        if (delay_ms < LVGL_TASK_MIN_DELAY_MS) delay_ms = LVGL_TASK_MIN_DELAY_MS;
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

static void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    uint16_t x = 0, y = 0;
    if (touch_dev->GetCoords(&x, &y)) {
        data->point.x = (lv_coord_t)LV_MIN(x, BSP_LCD_H_RES);
        data->point.y = (lv_coord_t)LV_MIN(y, BSP_LCD_V_RES);
        data->state   = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void display_reset_via_pmic(void) {
    Axp2101_SetAldo3(1); vTaskDelay(pdMS_TO_TICKS(100));
    Axp2101_SetAldo3(0); vTaskDelay(pdMS_TO_TICKS(100));
    Axp2101_SetAldo3(1); vTaskDelay(pdMS_TO_TICKS(100));
}

void bsp_lvgl_init(I2cMasterBus &i2cbus) {
    touch_dev = new LcdTouchPanel(i2cbus, 0x5A);
    lvgl_mux  = xSemaphoreCreateMutex();
    assert(lvgl_mux);

    /* SPI bus */
    spi_bus_config_t buscfg = {};
    buscfg.sclk_io_num      = BSP_LCD_PCLK;
    buscfg.data0_io_num     = BSP_LCD_DATA0;
    buscfg.data1_io_num     = BSP_LCD_DATA1;
    buscfg.data2_io_num     = BSP_LCD_DATA2;
    buscfg.data3_io_num     = BSP_LCD_DATA3;
    buscfg.max_transfer_sz  = BSP_LCD_H_RES * BSP_LCD_V_RES * LCD_BIT_PER_PIXEL / 8;
    ESP_ERROR_CHECK(spi_bus_initialize((spi_host_device_t)BSP_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO));

    /* Panel IO */
    esp_lcd_panel_io_spi_config_t io_config = {};
    io_config.cs_gpio_num        = BSP_LCD_CS;
    io_config.dc_gpio_num        = -1;
    io_config.spi_mode           = 0;
    io_config.pclk_hz            = 40 * 1000 * 1000;
    io_config.trans_queue_depth  = 2;
    io_config.lcd_cmd_bits       = 32;
    io_config.lcd_param_bits     = 8;
    io_config.flags.quad_mode    = true;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_NUM, &io_config, &io_handle));

    /* SH8601 panel */
    sh8601_vendor_config_t vendor_config = {};
    vendor_config.init_cmds               = lcd_init_cmds;
    vendor_config.init_cmds_size          = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]);
    vendor_config.flags.use_qspi_interface = 1;

    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.reset_gpio_num   = BSP_LCD_RST;
    panel_config.rgb_ele_order    = LCD_RGB_ELEMENT_ORDER_RGB;
    panel_config.bits_per_pixel   = LCD_BIT_PER_PIXEL;
    panel_config.vendor_config    = &vendor_config;

    ESP_ERROR_CHECK(esp_lcd_new_panel_sh8601(io_handle, &panel_config, &panel_handle));
    display_reset_via_pmic();
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    /* LVGL */
    lv_init();
    disp = lv_display_create(BSP_LCD_H_RES, BSP_LCD_V_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);

    uint8_t *buf1 = (uint8_t *)heap_caps_malloc(BUFF_SIZE, MALLOC_CAP_DMA);
    uint8_t *buf2 = (uint8_t *)heap_caps_malloc(BUFF_SIZE, MALLOC_CAP_DMA);
    assert(buf1 && buf2);
    lv_display_set_buffers(disp, buf1, buf2, BUFF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_user_data(disp, panel_handle);
    lv_display_add_event_cb(disp, my_disp_rounder, LV_EVENT_INVALIDATE_AREA, nullptr);

    /* Touch input */
    indev_drv = lv_indev_create();
    lv_indev_set_type(indev_drv, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_drv, my_touchpad_read);

    /* LVGL tick via esp_timer */
    esp_timer_create_args_t tick_args = {};
    tick_args.callback = increase_lvgl_tick;
    tick_args.name     = "lvgl_tick";
    esp_timer_handle_t tick_timer;
    ESP_ERROR_CHECK(esp_timer_create(&tick_args, &tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    xTaskCreate(lvgl_port_task, "LVGL", LVGL_TASK_STACK_SIZE, nullptr, LVGL_TASK_PRIORITY, nullptr);
}

void Lcd_SetBacklight(uint8_t brightness) {
    if (brightness > 100) brightness = 100;
    uint8_t  bl  = (uint8_t)((brightness * 255UL) / 100);
    uint32_t cmd = (0x02UL << 24) | (0x51 << 8);
    esp_lcd_panel_io_tx_param(io_handle, (int)cmd, &bl, 1);
}
