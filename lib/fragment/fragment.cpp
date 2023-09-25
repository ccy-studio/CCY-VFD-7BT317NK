/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-19 17:38:53
 * @LastEditTime: 2023-09-25 23:39:49
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
static u8 replace_page_flag = 0;
static void* replace_page_param = NULL;

void configuration_check();

void set_key_listener() {
    pinMode(KEY1, INPUT);
    pinMode(KEY2, INPUT);
    pinMode(KEY3, INPUT);
    // 注册按键中断函数
    attachInterrupt(digitalPinToInterrupt(KEY1), handle_key_interrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(KEY2), handle_key_interrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(KEY3), handle_key_interrupt, FALLING);
}

/**
 * 检查双击按键是否满足
 */
u8 check_double_press(u32 lasttime) {
    u32 diff = ((micros() - lasttime) / 1000);
    if (lasttime != 0 && diff > 50 && diff < 500) {
        // double click
        return 1;
    }
    return 0;
}

IRAM_ATTR void handle_key_interrupt() {
    btn_trigger();
    // 按下是低电平
    u32 filter_sec = (micros() - key_filter_sec) / 1000;
    if (filter_sec < 50) {
        btn_release();
        return;
    }
    if (digitalRead(KEY3) && !digitalRead(KEY1) && !digitalRead(KEY2)) {
        // 组合键 KEY1+KEY2按下
        active_obj->btn_callback(KEY1 | KEY2, BUTTON_ACTION_PRESS_DOWN);
    } else if (!digitalRead(KEY1)) {
        key_last_pin = KEY1;
        u8 btn_action = BUTTON_ACTION_PRESS_DOWN;
        if (check_double_press(key1_last_time)) {
            btn_action = BUTTON_ACTION_DOUBLE_PRESS;
        }
        key1_last_time = micros();
        key2_last_time = 0;
        key3_last_time = 0;
        active_obj->btn_callback(KEY1, btn_action);
    } else if (!digitalRead(KEY2)) {
        key_last_pin = KEY2;
        u8 btn_action = BUTTON_ACTION_PRESS_DOWN;
        if (check_double_press(key2_last_time)) {
            btn_action = BUTTON_ACTION_DOUBLE_PRESS;
        }
        key2_last_time = micros();
        key1_last_time = 0;
        key3_last_time = 0;
        active_obj->btn_callback(KEY2, btn_action);
    } else if (!digitalRead(KEY3)) {
        key_last_pin = KEY2;
        u8 btn_action = BUTTON_ACTION_PRESS_DOWN;
        if (check_double_press(key3_last_time)) {
            btn_action = BUTTON_ACTION_DOUBLE_PRESS;
        }
        key3_last_time = micros();
        key1_last_time = 0;
        key2_last_time = 0;
        active_obj->btn_callback(KEY3, btn_action);
    }
    key_filter_sec = micros();
    btn_release();
}

void fragment_init() {
    // 按键初始化
    set_key_listener();
    // 蜂鸣器初始化
    buzzer_setup(handle_key_interrupt);
    rx8025t_init(set_key_listener);
    rx8025_reset();
    // 初始化VFD
    send_global_event(EVENT_VFD_OPEN, NULL);
    for (size_t i = 0; i < PAGE_COUNT; i++) {
        fragmen_obj obj = fragment_page_arr[i];
        obj.on_create(NULL);
    }
    active_obj = &fragment_page_arr[0];
    active_obj->active = 1;
    active_obj->on_resume(NULL);
}

void fragment_loop() {
    if (replace_page_flag) {
        if (active_obj->fid != replace_page_flag) {
            for (size_t i = 0; i < PAGE_COUNT; i++) {
                fragmen_obj obj = fragment_page_arr[i];
                if (obj.fid == replace_page_flag &&
                    obj.fid != active_obj->fid) {
                    active_obj->on_pause(NULL);
                    active_obj->active = 0;
                    active_obj = &obj;
                    active_obj->on_resume(replace_page_param);
                    active_obj->active = 1;
                    break;
                }
            }
        }
        replace_page_flag = 0;
    }

    if (active_obj != NULL && active_obj->on_loop != NULL) {
        active_obj->on_loop(NULL);
    }
    configuration_check();
}

void replace_page(u8 fid, void* params) {
    replace_page_flag = fid;
    replace_page_param = params;
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
