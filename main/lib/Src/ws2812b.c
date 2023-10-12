/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-09 17:18:28
 * @LastEditTime: 2023-10-12 16:09:41
 */
#include "ws2812b.h"
#include <esp8266/gpio_struct.h>
#include "color.h"
#include "rgb.h"

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} rgb_cache;

rgb_cache cache_arr[RGB_LED_COUNT];

static u8 buf_arr[RGB_LED_COUNT * 3];

static u8 brightness_val, curr_style;

void reset() {
    gpio_set_level(RGB_PIN, 0);
    delay_ms(100);
}

void pin_low() {
    GPIO.out_w1ts |= (0x1 << RGB_PIN);  // 1
    GPIO.out_w1ts |= (0x1 << RGB_PIN);  // 1
    GPIO.out_w1tc |= (0x1 << RGB_PIN);  // 0
    GPIO.out_w1tc |= (0x1 << RGB_PIN);  // 0
    GPIO.out_w1tc |= (0x1 << RGB_PIN);  // 0
    GPIO.out_w1tc |= (0x1 << RGB_PIN);  // 0
}

void pin_high() {
    GPIO.out_w1ts |= (0x1 << RGB_PIN);  // 1
    GPIO.out_w1ts |= (0x1 << RGB_PIN);  // 1
    GPIO.out_w1ts |= (0x1 << RGB_PIN);  // 1
    GPIO.out_w1ts |= (0x1 << RGB_PIN);  // 1
    GPIO.out_w1tc |= (0x1 << RGB_PIN);  // 0
    GPIO.out_w1tc |= (0x1 << RGB_PIN);  // 0
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

// void send_grb(u8 g, u8 r, u8 b) {
//     send_arr(g);
//     send_arr(r);
//     send_arr(b);
// }

void rbg_init() {
    PIN_INIT_NONE_OUTPUT(RGB_PIN);
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

void rgb_update() {
    memset(buf_arr, 0, sizeof(buf_arr));
    u32 bi = 0;
    rgb_t rgb;
    for (size_t i = 0; i < RGB_LED_COUNT; i++) {
        rgb_cache cache = cache_arr[i];
        rgb.b = cache.b;
        rgb.r = cache.r;
        rgb.g = cache.g;
        rgb_scale_video(rgb, brightness_val);
        buf_arr[bi++] = rgb.g;
        buf_arr[bi++] = rgb.r;
        buf_arr[bi++] = rgb.b;
    }
    for (size_t i = 0; i < bi; i++) {
        u8 data = buf_arr[i];
        send_arr(data);
    }
    reset();

    // 高187ns 低 250ns
    // t0h400ns t0l850ns
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

void rgb_clear() {
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