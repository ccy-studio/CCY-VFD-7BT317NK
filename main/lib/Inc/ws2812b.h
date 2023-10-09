/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-09 17:15:16
 * @LastEditTime: 2023-10-09 17:56:34
 */
#ifndef WS2812B_H_
#define WS2812B_H_

#include "constant.h"
#include "color.h"

#define RGB_PIN 15
#define RGB_LED_COUNT 2  // WS2812B 数量

void rbg_init();

void rgb_set_color(u8 index, u8 r, u8 g, u8 b);

void rgb_update();

#endif