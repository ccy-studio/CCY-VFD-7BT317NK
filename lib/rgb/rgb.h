/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-18 09:29:52
 * @LastEditTime: 2023-08-18 10:19:43
 */
#ifndef __RGB_H
#define __RGB_H

#include <Arduino.h>
#include <FastLED.h>

#define RGB_LED_PIN 15   // WS2812B PIN
#define RGB_LED_COUNT 2  // WS2812B 数量

//显示样式
#define RGB_STYLE_1 1
#define RGB_STYLE_2 2
#define RGB_STYLE_3 3

/**
 * GPIO初始化
 */
void rgb_setup();

/**
 * 帧更新
 */
void rbg_frame_update();

/**
 * 设置显示样式 RGB_STYLE_X
 */
void rgb_set_style(u8 style);

/**
 * 清空
 */
void rgb_clear();

/**
 * 设置亮度 0~255
 */
void rgb_set_brightness(u8 brightness);

#endif