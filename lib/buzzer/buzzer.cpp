/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-18 10:23:59
 * @LastEditTime: 2023-08-18 10:46:57
 */
#include <buzzer.h>

FunctionIntRoutine save_routine;

void buzzer_setup(FunctionIntRoutine intRoutine) {
    save_routine = intRoutine;
}

void buzzer_out() {
    detachInterrupt(digitalPinToInterrupt(BUZZER_PIN));
    pinMode(BUZZER_PIN, OUTPUT);
}

void buzzer_in() {
    pinMode(BUZZER_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUZZER_PIN), save_routine, CHANGE);
}

void buzzer_play_di(u8 time) {
    buzzer_out();
    tone(BUZZER_PIN, 1000);
    delay(time);
    noTone(BUZZER_PIN);
    buzzer_in();
}