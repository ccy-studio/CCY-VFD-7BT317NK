/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-09 17:18:28
 * @LastEditTime: 2023-10-09 17:58:31
 */
#include "ws2812b.h"

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} rgb_cache;

rgb_cache cache_arr[RGB_LED_COUNT];

void delay_ns(u32 ns) {
    while (ns--)
        ;
}

void reset() {
    gpio_set_level(RGB_PIN, 0);
    delay_ms(350);
}

void pin_low() {
    gpio_set_level(RGB_PIN, 1);
    delay_ns(300);
    gpio_set_level(RGB_PIN, 0);
    delay_ns(1090);
}

void pin_high() {
    gpio_set_level(RGB_PIN, 1);
    delay_ns(1090);
    gpio_set_level(RGB_PIN, 0);
    delay_ns(320);
}

void send_arr(u8 buf) {
    uint8_t i;
    for (i = 8; i > 0; i--) {
        if (buf & 0x80) {
            pin_high();
        } else {
            pin_low();
        }
        buf <<= 1;
    }
}

void send_grb(u8 g, u8 r, u8 b) {
    send_arr(g);
    send_arr(r);
    send_arr(b);
}

void rbg_init() {
    PIN_INIT_NONE_OUTPUT(RGB_PIN);
}

void rgb_set_color(u8 index, u8 r, u8 g, u8 b) {
    if (index >= RGB_LED_COUNT) {
        ESP_LOGE(TAG, "错误不能超过最大LED数量限制!");
        return;
    }
    cache_arr[index].r = r;
    cache_arr[index].g = g;
    cache_arr[index].b = b;
}

void rgb_update() {
    for (size_t i = 0; i < RGB_LED_COUNT; i++) {
        rgb_cache cache = cache_arr[i];
        send_grb(cache.g, cache.r, cache.b);
    }
    reset();
}