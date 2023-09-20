#ifndef __FRAGMENT_H
#define __FRAGMENT_H

#include <buzzer.h>
#include "constant.h"
#include "thread_helper.h"
#include <gui.h>
#include "store.h"
#include "rx8025.h"
#include "rgb.h"

#define FRAGMENT_PAGE_SETTING 0x10
#define FRAGMENT_PAGE_CLOCK 0x00

/**
 * 按键类型宏定义常量
 */
#define BUTTON_ACTION_PRESS_DOWN 0x01  // 按下
#define BUTTON_ACTION_PRESS_UP 0x02    // 松开
#define BUTTON_ACTION_PRESS_LONG 0x03  // 长按


typedef void (*fun_button_click_callback)(u8 btn_key, u8 btn_action);
typedef void (*fun_active_on_resume)(void* params);
typedef void (*fun_active_on_pause)(void* params);
typedef void (*fun_active_on_loop)(void* params);
typedef void (*fun_active_on_create)(void* params);
typedef void (*fun_active_on_event)(u8 event_id, void* params);

typedef struct{
    u8 fid;  // 页面标识ID
    u8 active = 0;
    fun_button_click_callback btn_callback = NULL;
    fun_active_on_create on_create = NULL;
    fun_active_on_resume on_resume = NULL;
    fun_active_on_pause on_pause = NULL;
    fun_active_on_loop on_loop = NULL;
    fun_active_on_event on_event = NULL;
} fragmen_obj;

#define PAGE_COUNT 2

void fragment_init();
void fragment_loop();
void replace_page(u8 fid, void* params = NULL);
/**
 * 发送全局事件
 */
void send_global_event(u8 event_id, void* params = NULL);

#endif