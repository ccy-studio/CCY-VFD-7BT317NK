/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 10:07:21
 * @LastEditTime: 2023-10-08 10:07:38
 */

#ifndef __STORE_H
#define __STORE_H

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