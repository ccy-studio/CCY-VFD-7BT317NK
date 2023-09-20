/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-19 17:38:53
 * @LastEditTime: 2023-09-20 17:27:52
 */
#include "fragment.h"

extern fragmen_obj fragment_page_arr[PAGE_COUNT];
///--------------------KEY-Start-----------
u32 key_filter_sec = 0;  // 按键防抖
u32 key1_last_time = 0;  // 按键1的上一次按下触发时间记录
u32 key2_last_time = 0;  // 按键2的上一次按下触发时间记录
u32 key3_last_time = 0;  // 按键3的上一次按下触发时间记录
u8 key_last_pin = 0;     // 记录上一次按下的按键PIN
IRAM_ATTR void handle_key_interrupt();
///--------------------KEY-END-----------

fragmen_obj* active_obj;  // 当前激活的Page | 页面

void configuration_check();

void set_key_listener() {
    pinMode(KEY1, INPUT);
    pinMode(KEY2, INPUT);
    pinMode(KEY3, INPUT);
    // 注册按键中断函数
    attachInterrupt(digitalPinToInterrupt(KEY1), handle_key_interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(KEY2), handle_key_interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(KEY3), handle_key_interrupt, CHANGE);
}

IRAM_ATTR void handle_key_interrupt() {
    // 按下是低电平
    u32 filter_sec = (micros() - key_filter_sec) / 1000;
    if (filter_sec < 500) {
        return;
    }
    if (digitalRead(KEY3) && !digitalRead(KEY1) && !digitalRead(KEY2)) {
        // 组合键 KEY1+KEY2按下
        active_obj->btn_callback(KEY1 | KEY2, BUTTON_ACTION_PRESS_DOWN);
    } else if (!digitalRead(KEY1)) {
        key_last_pin = KEY1;
        key1_last_time = micros();
        key2_last_time = 0;
        key3_last_time = 0;
        active_obj->btn_callback(KEY1, BUTTON_ACTION_PRESS_DOWN);
    } else if (!digitalRead(KEY2)) {
        key_last_pin = KEY2;
        key2_last_time = micros();
        key1_last_time = 0;
        key3_last_time = 0;
        active_obj->btn_callback(KEY2, BUTTON_ACTION_PRESS_DOWN);
    } else if (!digitalRead(KEY3)) {
        key_last_pin = KEY2;
        key3_last_time = micros();
        key1_last_time = 0;
        key2_last_time = 0;
        active_obj->btn_callback(KEY3, BUTTON_ACTION_PRESS_DOWN);
    }
    // check long press
    if (digitalRead(KEY3) && digitalRead(KEY2) && digitalRead(KEY1)) {
        u32 diff = 0;
        if (key_last_pin == KEY3) {
            diff = (micros() - key3_last_time) / 1000;
            if (key3_last_time && diff > 2000) {
                active_obj->btn_callback(KEY3, BUTTON_ACTION_PRESS_LONG);
            } else {
                active_obj->btn_callback(KEY3, BUTTON_ACTION_PRESS_UP);
            }
        } else if (key_last_pin == KEY2) {
            diff = (micros() - key2_last_time) / 1000;
            if (key2_last_time && diff > 2000) {
                active_obj->btn_callback(KEY2, BUTTON_ACTION_PRESS_LONG);
            } else {
                active_obj->btn_callback(KEY2, BUTTON_ACTION_PRESS_UP);
            }
        } else if (key_last_pin == KEY1) {
            diff = (micros() - key1_last_time) / 1000;
            if (key1_last_time && diff > 2000) {
                active_obj->btn_callback(KEY1, BUTTON_ACTION_PRESS_LONG);
            } else {
                active_obj->btn_callback(KEY1, BUTTON_ACTION_PRESS_UP);
            }
        }
    }
    key_filter_sec = micros();
}

void fragment_init() {
    // 按键初始化
    set_key_listener();
    // 蜂鸣器初始化
    buzzer_setup(handle_key_interrupt);
    rx8025t_init(set_key_listener);
    // 初始化VFD
    send_global_event(EVENT_VFD_OPEN);
    for (size_t i = 0; i < PAGE_COUNT; i++) {
        fragmen_obj obj = fragment_page_arr[i];
        obj.on_create(NULL);
    }
    active_obj = &fragment_page_arr[0];
    active_obj->active = 1;
    active_obj->on_resume(NULL);
}

void fragment_loop() {
    configuration_check();
    if (active_obj != NULL) {
        active_obj->on_loop(NULL);
    }
}

void replace_page(u8 fid, void* params) {
    if (active_obj->fid == fid) {
        return;
    }
    for (size_t i = 0; i < PAGE_COUNT; i++) {
        fragmen_obj obj = fragment_page_arr[i];
        if (obj.fid == fid && obj.fid != active_obj->fid) {
            active_obj->on_pause(NULL);
            active_obj->active = 0;
            active_obj = &obj;
            active_obj->on_resume(params);
            active_obj->active = 1;
            return;
        }
    }
}

void handler_vfd_controll(u8 event_id, void* params = NULL);
void handler_rgb_controll(u8 event_id);
void handelr_g1_anno_controll(u8 event_id);
void send_global_event(u8 event_id, void* params) {
    if (event_id | 0x10) {
        // VFD操作
        handler_vfd_controll(event_id, params);
        return;
    }
    if (event_id | 0x20) {
        // RGB操作
        handler_rgb_controll(event_id);
        return;
    }
    if (event_id | 0x30) {
        // G1动画操作
        handelr_g1_anno_controll(event_id);
        return;
    }
    for (size_t i = 0; i < PAGE_COUNT; i++) {
        fragmen_obj obj = fragment_page_arr[i];
        if (obj.on_event != NULL) {
            obj.on_event(event_id, params);
        }
    }
}
