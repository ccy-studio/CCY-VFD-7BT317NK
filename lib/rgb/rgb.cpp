/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-18 09:29:57
 * @LastEditTime: 2023-08-19 23:49:45
 */
#include <rgb.h>

typedef struct rgb_led {
    u8 r;
    u8 g;
    u8 b;
    u8 i_r = 1;
    u8 i_g = 1;
    u8 i_b = 1;
};

CRGB leds[RGB_LED_COUNT];

rgb_led rgb_led_obj[RGB_LED_COUNT];

u8 rgb_style = RGB_STYLE_1;  // 显示样式

u8 rgb_brightness = 255;  // 亮度

void rgb_setup() {
    pinMode(RGB_LED_PIN, OUTPUT);
    // WS2812B 初始化
    FastLED.addLeds<WS2812B, RGB_LED_PIN, RGB>(leds, RGB_LED_COUNT)
        .setCorrection(TypicalSMD5050);
    // 亮度 0~255
    FastLED.setBrightness(255);
}

void rgb_set_style(u8 style) {
    rgb_style = style;
}

void rgb_set_val(rgb_led* led) {
    if (led->r >= 255) {
        led->i_r = 0;
    } else if (led->r <= 0) {
        led->i_r = 1;
    }

    if (led->g >= 255) {
        led->i_g = 0;
    } else if (led->g <= 0) {
        led->i_g = 1;
    }

    if (led->b >= 255) {
        led->i_b = 0;
    } else if (led->b <= 0) {
        led->i_b = 1;
    }

    led->r = led->i_r ? led->r + 1 : led->r - 1;
    led->g = led->i_g ? led->g + 1 : led->g - 1;
    led->b = led->i_b ? led->b + 1 : led->b - 1;
}

void rbg_frame_update() {
    if (rgb_style == RGB_STYLE_1) {
        for (u8 i = 0; i < RGB_LED_COUNT; i++) {
            rgb_led rgb = rgb_led_obj[i];
            rgb_set_val(&rgb);
            leds[i].setRGB(rgb.r, rgb.g, rgb.b);
        }
    } else if (rgb_style == RGB_STYLE_2) {
        static u8 hue = 0;
        fill_rainbow(leds, RGB_LED_COUNT, hue++, 10);
    } else if (rgb_style == RGB_STYLE_3) {
    }
    FastLED.setBrightness(rgb_brightness);
    FastLED.show();
}

void rgb_set_brightness(u8 brightness) {
    rgb_brightness = brightness;
    rgb_clear();
}

void rgb_clear() {
    FastLED.clear();
}