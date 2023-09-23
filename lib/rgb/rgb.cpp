/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-19 17:29:48
 * @LastEditTime: 2023-09-23 21:57:55
 */
#include <rgb.h>

Adafruit_NeoPixel strip(RGB_LED_COUNT, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

u8 rgb_style = RGB_STYLE_1;  // 显示样式

u8 rgb_brightness = 255;  // 亮度

void rgb_setup() {
    pinMode(RGB_LED_PIN, OUTPUT);

    strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();   // Turn OFF all pixels ASAP
    strip.setBrightness(
        rgb_brightness);  // Set BRIGHTNESS to about 1/5 (max = 255)
}

void rgb_set_style(u8 style) {
    rgb_style = style;
}

void rbg_frame_update() {
    if (rgb_style == RGB_STYLE_1) {
        // static u8 hue1 = 0;
        // // 渐变呼吸
        // uint8_t brightness = beatsin8(
        //     40, 0,
        //     rgb_brightness);  // 生成一个60秒周期，0到255范围内变化的亮度值
        // leds[0] = CHSV(hue1, 255, brightness);
        // leds[1] = CHSV(hue1, 255, brightness);
        // hue1++;
    } else if (rgb_style == RGB_STYLE_2) {
        // 彩虹渐变
        static u16 hue = 0;
        strip.rainbow(hue);
        hue += 256;
        strip.setBrightness(rgb_brightness);
    } else if (rgb_style == RGB_STYLE_3) {
        // static u8 direction = 1;
        // for (int i = 0; i < RGB_LED_COUNT; i++) {
        //     if (direction) {
        //         leds[i] = CRGB::White;
        //     } else {
        //         leds[i] = CRGB::Black;
        //     }
        // }
        // direction = !direction;
    }
    strip.show();
}

void rgb_set_brightness(u8 brightness) {
    rgb_brightness = brightness;
}

void rgb_clear() {
    strip.setBrightness(0);
    strip.clear();
}