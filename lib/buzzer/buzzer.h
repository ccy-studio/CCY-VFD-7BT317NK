#ifndef __BUZZER_H
#define __BUZZER_H

#include <Arduino.h>

#define BUZZER_PIN 2

typedef void (*FunctionIntRoutine)();

void buzzer_setup(FunctionIntRoutine intRoutine);

/**
 * 设置为OUTPUT
 */
void buzzer_out();

/**
 * 设置为INPUT
 */
void buzzer_in();

/**
 * 发出滴的声音
 * @param time 播放时长/ms
 */
void buzzer_play_di(u8 time = 200);

#endif