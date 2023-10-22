/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 15:48:25
 * @LastEditTime: 2023-10-08 18:03:53
 */
#include "buzzer.h"
#include "driver/hw_timer.h"

#define BUZZER_PERIOD 600

static fun_key_action key_func;

static void hw_timer_callback(void *arg) {
    static u8 state = 0;
    gpio_set_level(BUZZER_PIN, (state++) % 2);
}

void buzzer_bind(fun_key_action fun) {
    key_func = fun;
}

void buzzer_fast_play(u32 duration) {
    key_func(1);
    // action pwm
    PIN_INIT_NONE_OUTPUT(BUZZER_PIN);
    hw_timer_init(hw_timer_callback, NULL);
    hw_timer_alarm_us(BUZZER_PERIOD, true);
    delay_ms(duration);
    hw_timer_disarm();
    hw_timer_deinit();
    key_func(0);
}