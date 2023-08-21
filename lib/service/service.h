#ifndef __SERVICE_H
#define __SERVICE_H

#include <Arduino.h>
#include <constant.h>

typedef void (*AutoPowerCallback)(u8 state);

/**
 * 定时开关机
 */
void logic_handler_auto_power(tm* timeinfo,AutoPowerCallback callback);

/**
 * 闹钟
 */
void logic_handler_alarm_clock(tm* timeinfo);

/**
 * 计时器
 */
void logic_handler_countdown(tm* timeinfo);

#endif