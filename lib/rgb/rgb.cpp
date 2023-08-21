/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-18 09:29:57
 * @LastEditTime: 2023-08-21 23:45:42
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
    randomSeed(analogRead(0));  // 使用模拟引脚读取来设置随机种子
}

void rgb_set_style(u8 style) {
    rgb_style = style;
}

void rbg_frame_update() {
    if (rgb_style == RGB_STYLE_1) {
        static u8 hue1 = 0;
        // 渐变呼吸
        uint8_t brightness = beatsin8(
            40, 0,
            rgb_brightness);  // 生成一个60秒周期，0到255范围内变化的亮度值
        leds[0] = CHSV(hue1, 255, brightness);
        leds[1] = CHSV(hue1, 255, brightness);
        hue1++;
    } else if (rgb_style == RGB_STYLE_2) {
        // 彩虹渐变
        static u8 hue = 0;
        fill_rainbow(leds, RGB_LED_COUNT, hue++, 10);
        FastLED.setBrightness(rgb_brightness);
    } else if (rgb_style == RGB_STYLE_3) {
    }
    FastLED.show();
}

void rgb_set_brightness(u8 brightness) {
    rgb_brightness = brightness;
}

void rgb_clear() {
    FastLED.setBrightness(0);
    FastLED.clear(true);
}