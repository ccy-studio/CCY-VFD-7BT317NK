/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-09 17:18:28
 * @LastEditTime: 2023-10-09 17:58:31
 */
#include <esp8266/gpio_struct.h>
#include "ws2812b.h"

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} rgb_cache;

rgb_cache cache_arr[RGB_LED_COUNT];

static u8 buf_arr[RGB_LED_COUNT * 3];


void reset() {
    gpio_set_level(RGB_PIN, 0);
    delay_ms(100);
}

void pin_low() {
    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
}

void pin_high() {
    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
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

//void send_grb(u8 g, u8 r, u8 b) {
//    send_arr(g);
//    send_arr(r);
//    send_arr(b);
//}

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
    memset(buf_arr, 0, sizeof(buf_arr));
    u32 bi = 0;
    for (size_t i = 0; i < RGB_LED_COUNT; i++) {
        rgb_cache cache = cache_arr[i];
        buf_arr[bi++] = cache.g;
        buf_arr[bi++] = cache.r;
        buf_arr[bi++] = cache.b;
    }
    for (size_t i = 0; i < bi; i++) {
        u8 data = buf_arr[i];
        send_arr(data);
    }
    reset();

    //高187ns 低 250ns
    //t0h400ns t0l850ns
//    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
//    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
//    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
//    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
//    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
//    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
//
//    //t1h800ns t1l450ns
//    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
//    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
//    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
//    GPIO.out_w1ts |= (0x1 << RGB_PIN); //1
//    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
//    GPIO.out_w1tc |= (0x1 << RGB_PIN);//0
}