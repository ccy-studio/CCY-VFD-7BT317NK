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
 * @Date: 2023-08-18 15:43:27
 * @LastEditTime: 2023-08-21 21:12:37
 */
#ifndef __STORE_H
#define __STORE_H

#include <Arduino.h>
#include <constant.h>

typedef struct {
    u8 anno_open = 1;         // G1动画开关
    u8 rgb_open = 1;          // RGB灯效开关
    u8 rgb_style = 2;         // RGB灯效样式 RGB_STYLE_2
    u8 rgb_brightness = 128;  // RGB亮度

    char custom_long_text[50] = {"Hello VFD"};  // 滚动文字
    u8 custom_long_text_frame = 255;            // 滚动文字帧率

    u8 auto_power;                  // 定时开关机开关
    char auto_power_open_time[9];   // 开机时间设定
    char auto_power_close_time[9];  // 关机时间设定
    u8 auto_power_enable_days[7];   // 开关机生效条件

    u8 alarm_clock;                 // 闹钟开关
    char alarm_clock_time[9];       // 闹钟定时时间
    u8 alarm_clock_enable_days[7];  // 闹钟生效条件

    u8 countdown;            // 倒计时
    char countdown_time[9];  // 倒计时时间
} store_setting_obj;

extern store_setting_obj setting_obj;

void store_init();

void store_close();

void store_save_setting(store_setting_obj* obj);

void store_get_setting(store_setting_obj* obj);

void store_del_setting(void);

#ifdef DEBUG
void store_print_debug(store_setting_obj setting_obj);
#endif

#endif