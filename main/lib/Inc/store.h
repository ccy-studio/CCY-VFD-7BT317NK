/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 10:07:21
 * @LastEditTime: 2023-10-10 16:22:53
 */

#ifndef __STORE_H
#define __STORE_H

#include <constant.h>

typedef struct {
    u8 anno_open;       // G1动画开关
    u8 rgb_open;        // RGB灯效开关
    u8 rgb_style;       // RGB灯效样式 RGB_STYLE_2
    u8 rgb_brightness;  // RGB亮度

    char custom_long_text[50];  // 滚动文字
    u32 custom_long_text_frame;  // 滚动文字帧率

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

extern store_setting_obj glob_setting_config;

void store_init();

void store_close();

void store_save_setting();

store_setting_obj *get_store();

void store_del_setting(void);

#ifdef DEBUG

void store_print_debug();

#endif

#endif