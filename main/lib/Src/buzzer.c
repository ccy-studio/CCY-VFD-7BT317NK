/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 15:48:25
 * @LastEditTime: 2023-10-08 17:57:05
 */
#include "buzzer.h"
#include "driver/ledc.h"

#define BUZZER_PERIOD 1000

static fun_key_action key_func;

static const ledc_timer_config_t ledc_buzzer_timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,  // 定时器模式（“高速”||“低速”）
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num =
        LEDC_TIMER_0,  // ledc使用的定时器编号。若需要生成多个频率不同的PWM信号，则需要指定不同的定时器
                       // （定时器源 0-3）
    .freq_hz = 1000,   // PWM频率
};

static const ledc_channel_config_t ledc_buzzer_channel = {
    .gpio_num = BUZZER_PIN,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .duty = 500};

void buzzer_bind(fun_key_action fun) {
    key_func = fun;
    CHECK_LOGE(ledc_timer_config(&ledc_buzzer_timer), "ledc配置错误");
}
void buzzer_fast_play(u8 duration) {
    key_func(0);
    // action pwm
    key_func(1);
}