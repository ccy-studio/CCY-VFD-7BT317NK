/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-09 17:18:28
 * @LastEditTime: 2023-10-20 16:22:58
 */
#include "ws2812b.h"
#include "color.h"
#include "driver/spi.h"
#include "rgb.h"

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} rgb_cache;

volatile rgb_cache cache_arr[RGB_LED_COUNT];
static u8 buf_arr[RGB_LED_COUNT * 3];
static volatile u8 brightness_val, curr_style;

void rgb_init() {
    spi_config_t spi_config;
    // Load default interface parameters
    // CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1,
    // BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
    spi_config.interface.val = SPI_DEFAULT_INTERFACE;
    // Load default interrupt enable
    // TRANS_DONE: true, WRITE_STATUS: false, READ_STATUS: false, WRITE_BUFFER:
    // false, READ_BUFFER: false
    spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
    spi_config.interface.cs_en = 0;
    spi_config.interface.miso_en = 0;
    // CPOL: 1, CPHA: 1
    spi_config.interface.cpol = 1;
    spi_config.interface.cpha = 1;
    // Set SPI to master mode
    // 8266 Only support half-duplex
    spi_config.mode = SPI_MASTER_MODE;
    // Set the SPI clock frequency division factor
    spi_config.clk_div = SPI_8MHz_DIV;
    spi_config.event_cb = NULL;
    spi_init(HSPI_HOST, &spi_config);
}

void rgb_set_color(u8 index, u8 r, u8 g, u8 b) {
    if (index >= RGB_LED_COUNT) {
        ESP_LOGE(APP_TAG, "错误不能超过最大LED数量限制!");
        return;
    }
    cache_arr[index].r = r;
    cache_arr[index].g = g;
    cache_arr[index].b = b;
}

static send_buf_spi() {
    u32 len = RGB_LED_COUNT * 3;
    u32 blen = len * 8;
    u8 buf[blen];
    u32 pi = 0;
    for (size_t i = 0; i < len; i++) {
        u8 data = buf_arr[i];
        for (u8 j = 0; j < 8; j++) {
            if (data & 0x80) {
                buf[pi++] = 0xFC; /*11111100b;*/
            } else {
                buf[pi++] = 0XC0; /*11000000b;*/
            }
            data <<= 1;
        }
    }
    uint32_t* spi_buf = (uint32_t*)buf;
    spi_trans_t trans = {0};
    trans.mosi = spi_buf;
    trans.bits.mosi = blen * 8;
    spi_trans(HSPI_HOST, &trans);
}

void rgb_update() {
    memset(buf_arr, 0, sizeof(buf_arr));
    u32 bi = 0;
    rgb_t rgb;
    for (size_t i = 0; i < RGB_LED_COUNT; i++) {
        rgb_cache cache = cache_arr[i];
        rgb.b = cache.b;
        rgb.r = cache.r;
        rgb.g = cache.g;
        rgb.r = (uint8_t)(((uint16_t)rgb.r * brightness_val) / 255);
        rgb.g = (uint8_t)(((uint16_t)rgb.g * brightness_val) / 255);
        rgb.b = (uint8_t)(((uint16_t)rgb.b * brightness_val) / 255);
        buf_arr[bi++] = rgb.g;
        buf_arr[bi++] = rgb.r;
        buf_arr[bi++] = rgb.b;
    }
    send_buf_spi();
}

void rgb_clear() {
    brightness_val = 0;
    for (size_t i = 0; i < RGB_LED_COUNT; i++) {
        rgb_cache cache = cache_arr[i];
        cache.r = 0;
        cache.g = 0;
        cache.b = 0;
    }
    rgb_update();
}

uint32_t led_effect_color_wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        return ((uint32_t)(255 - pos * 3) << 16) | ((uint32_t)(0) << 8) |
               (pos * 3);
    } else if (pos < 170) {
        pos -= 85;
        return ((uint32_t)(0) << 16) | ((uint32_t)(pos * 3) << 8) |
               (255 - pos * 3);
    } else {
        pos -= 170;
        return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8) |
               (0);
    }
}

rgb_t rainbow(u8 pos) {
    uint32_t next_color;
    rgb_t next_pixel;

    next_color = led_effect_color_wheel(pos);
    next_pixel.r = (next_color >> 16) & 0xff;
    next_pixel.g = (next_color >> 8) & 0xff;
    next_pixel.b = (next_color);
    return next_pixel;
}

void rgb_fun_anno_update() {
    static u8 c_val;
    static rgb_t rgb;
    static u8 old_br = 0;
    if (curr_style == RGB_STYLE_1) {
        rgb = rainbow((c_val += 5));
        rgb_set_color(0, rgb.r, rgb.g, rgb.b);
        rgb = rainbow((c_val += 5));
        rgb_set_color(1, rgb.r, rgb.g, rgb.b);
    } else if (curr_style == RGB_STYLE_2) {
        rgb = rgb_heat_color((c_val += 10));
        rgb_set_color(0, rgb.r, rgb.g, rgb.b);
        rgb_set_color(1, rgb.r, rgb.g, rgb.b);
    } else if (curr_style == RGB_STYLE_3) {
    }
    rgb_update();
}

void rgb_fun_set_style(u8 style) {
    curr_style = style;
}

void rgb_fun_set_brightness(u8 brightness) {
    brightness_val = brightness;
}