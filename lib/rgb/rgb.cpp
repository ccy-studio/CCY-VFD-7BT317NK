/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-22 13:54:14
 * @LastEditTime: 2023-08-23 10:08:45
 */
/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系
 *[yustart@foxmail.com] 进行授权。
 *
 * 在遵循以下条件的情况下，您可以自由修改、使用和分发本代码：
 * - 您必须保留此版权声明的所有内容。
 * - 您不得用于任何违法、滥用、恶意的活动。
 * - 您的使用不应损害任何个人、组织或作者的声誉。
 *
 * 作者不承担因使用本代码而产生的任何担保责任。作者对因使用本代码所导致的任何直接或间接损失不承担责任。
 * Github: https://github.com/ccy-studio/CCY-VFD-7BT317NK
 ***********************************************************************************************/

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
        static u8 direction = 1;
        for (int i = 0; i < RGB_LED_COUNT; i++) {
            if (direction) {
                leds[i] = CRGB::White;
            } else {
                leds[i] = CRGB::Black;
            }
        }
        direction = !direction;
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