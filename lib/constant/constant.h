/*
 * @Description: 
 * @Author: chenzedeng
 * @Date: 2023-09-23 00:04:37
 * @LastEditTime: 2023-09-24 00:34:23
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
 * @Date: 2023-08-21 14:57:56
 * @LastEditTime: 2023-08-22 16:29:47
 */
#ifndef __CONSTANT_H
#define __CONSTANT_H

#include <Arduino.h>

// 是否开启debug模式打印日志,不需要请注释掉
#define DEBUG

// 按键PIN脚
#define KEY1 5
#define KEY2 4
#define KEY3 2

#define I2C_SDA 4
#define I2C_SCL 5

/**
 * G1动画帧率
 */
#define G1_ANNO_FRAME 150

/**
 * RGB帧率
 */
#define RGB_ANNO_FRAME 5

/**
 * VFD时间刷新帧率
 */
#define VFD_TIME_FRAME 500


#define NTP3 "ntp.ntsc.ac.cn"
// #define NTP2 "cn.ntp.org.cn"
// #define NTP1 "ntp.tuna.tsinghua.edu.cn"

#define EVENT_VFD_OPEN 0x10
#define EVENT_VFD_CLOSE 0x1f
#define EVENT_VFD_BRIGHTNESS_DECREASE 0x11
#define EVENT_VFD_BRIGHTNESS_INCREASE 0x12

#define EVENT_RGB_OPEN 0x20
#define EVENT_RGB_CLOSE 0x2f

#define EVENT_G1_OPEN 0x30
#define EVENT_G1_CLOSE 0x3f

#endif