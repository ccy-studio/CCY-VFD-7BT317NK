/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-09 17:15:16
 * @LastEditTime: 2023-10-12 15:43:52
 */
#ifndef WS2812B_H_
#define WS2812B_H_

#include "constant.h"

#define RGB_PIN 15
#define RGB_LED_COUNT 2  // WS2812B 数量

#define RGB_STYLE_1 0
#define RGB_STYLE_2 1
#define RGB_STYLE_3 2

void rbg_init();

void rgb_set_color(u8 index, u8 r, u8 g, u8 b);

void rgb_update();

void rgb_clear();

void rgb_fun_anno_update();
void rgb_fun_set_style(u8 style);
void rgb_fun_set_brightness(u8 brightness);

#endif