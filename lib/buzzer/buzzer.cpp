/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系 [yustart@foxmail.com] 进行授权。
 *
 * 在遵循以下条件的情况下，您可以自由修改、使用和分发本代码：
 * - 您必须保留此版权声明的所有内容。
 * - 您不得用于任何违法、滥用、恶意的活动。
 * - 您的使用不应损害任何个人、组织或作者的声誉。
 *
 * 作者不承担因使用本代码而产生的任何担保责任。作者对因使用本代码所导致的任何直接或间接损失不承担责任。
 * Github: https://github.com/ccy-studio/CCY-VFD-7BT317NK
 ***********************************************************************************************/

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