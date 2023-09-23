/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系 [yustart@foxmail.com] 进行授权。
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
 * @Date: 2023-08-18 09:29:52
 * @LastEditTime: 2023-08-18 10:19:43
 */
#ifndef __RGB_H
#define __RGB_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

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