#ifndef __SERVICE_H
#define __SERVICE_H

#include <Arduino.h>
#include <constant.h>

typedef void (*AutoPowerCallback)(u8 state);
typedef void (*AlarmClockCallback)(u8 state);
typedef void (*CountdownCallback)(u8 state, u8 hour, u8 min, u8 sec);

/**
 * 定时开关机
 */
void logic_handler_auto_power(tm* timeinfo, AutoPowerCallback callback);

/**
 * 闹钟
 */
void logic_handler_alarm_clock(tm* timeinfo, AlarmClockCallback callback);

/**
 * 计时器
 */
void logic_handler_countdown(tm* timeinfo,CountdownCallback callback);
void logic_handler_countdown_stop();

#endif