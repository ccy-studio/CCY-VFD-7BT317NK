#ifndef __BUZZER_H
#define __BUZZER_H

#include "constant.h"

#define BUZZER_PIN 2

void buzzer_bind(fun_key_action fun);

void buzzer_fast_play(u32 duration);

#endif