/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-09-24 11:37:07
 * @LastEditTime: 2023-09-25 09:47:32
 */
#include "rx8025.h"

// 时间补偿获取
rx8025_timeinfo last_timeinfo, current_timeinfo;

static u8 btn_press = 0;

void btn_trigger() {
    last_timeinfo = current_timeinfo;
    btn_press = 1;
}

void btn_release() {
    delayMicroseconds(2);
    rx8025_time_get(&current_timeinfo);
    btn_press = 0;
}

void rtc_update() {
    if (btn_press == 0) {
        rx8025_time_get(&current_timeinfo);
    }
}

rx8025_timeinfo* rtc_get_timeinfo() {
    return btn_press ? &last_timeinfo : &current_timeinfo;
}