/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-19 17:38:53
 * @LastEditTime: 2023-10-12 15:25:18
 */
#include "fragment.h"

extern fragmen_obj fragment_page_arr[PAGE_COUNT];
extern volatile bool power_state; //开关机状态
extern void auto_power_handle(u8 state);


void fragment_bind();

///--------------------KEY-Start-----------
static button_t btn_key1;
static button_t btn_key2;
static button_t btn_key3;
static button_t btn_receive;
///--------------------KEY-END-----------

EventGroupHandle_t fragment_event_handle = NULL;

fragmen_obj *active_obj;  // 当前激活的Page | 页面
static volatile u8 replace_page_flag = 0;
static void *replace_page_param = NULL;

void fragment_service_init();

void set_key_listener() {
    btn_key1.gpio = KEY1;
    button_add_btn(&btn_key1);

    btn_key2.gpio = KEY2;
    button_add_btn(&btn_key2);

    btn_key3.gpio = KEY3;
    button_add_btn(&btn_key3);
}

void buzz_key_handle(u8 state) {
    if (state) {
        // 使用IO
        button_remove_btn(&btn_key3);
    } else {
        // 释放
        button_add_btn(&btn_key3);
    }
}

void rx8025_key_handle(u8 state) {
    if (state) {
        // 使用IO
        button_remove_btn(&btn_key1);
        button_remove_btn(&btn_key2);
    } else {
        // 释放
        button_add_btn(&btn_key1);
        button_add_btn(&btn_key2);
    }
}

void fragment_init() {
    fragment_bind();
    fragment_event_handle = xEventGroupCreate();
    if (NULL == fragment_event_handle) {
        ESP_LOGE(APP_TAG, "事件创建失败!");
        return;
    }
    // 蜂鸣器初始化
    buzzer_bind(buzz_key_handle);
    rgb_init();
    rx8025t_init(rx8025_key_handle);
    rx8025_reset();
    // 按键初始化
    set_key_listener();
    button_init();
    // 初始化VFD
    xEventGroupSetBits(fragment_event_handle, EVENT_VFD_OPEN);
    fragment_service_init();
    for (size_t i = 0; i < PAGE_COUNT; i++) {
        fragmen_obj *obj = &fragment_page_arr[i];
        obj->on_create(NULL);
    }
    active_obj = &fragment_page_arr[0];
    active_obj->active = 1;
    active_obj->on_resume(NULL);
}

void fragment_loop() {
    if (xQueueReceive(btn_queue, &btn_receive, 0) == pdTRUE) {
        if (power_state) {
            if (active_obj != NULL) {
                active_obj->btn_callback(btn_receive.gpio, btn_receive.state);
            }
        } else {
            //关机唤醒
            auto_power_handle(1);
        }
    }
    if (replace_page_flag) {
        if (active_obj->fid != replace_page_flag) {
            for (size_t i = 0; i < PAGE_COUNT; i++) {
                fragmen_obj *obj = &fragment_page_arr[i];
                if (obj->fid == replace_page_flag) {
                    active_obj->on_pause(NULL);
                    delay_ms(5);
                    active_obj->active = 0;
                    active_obj = obj;
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
}

void replace_page(u8 fid, void *params) {
    replace_page_flag = fid;
    replace_page_param = params;
}
