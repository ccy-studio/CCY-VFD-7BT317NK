/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-19 17:38:33
 * @LastEditTime: 2023-10-18 15:26:39
 */
#ifndef __FRAGMENT_H
#define __FRAGMENT_H

#include <buzzer.h>
#include <gui.h>
#include "constant.h"
#include "ws2812b.h"
#include "rx8025.h"
#include "store.h"
#include "button.h"
#include "smart_wifi.h"
#include "service.h"

/**
 * 页面ID常量
 */
#define FRAGMENT_PAGE_SETTING 3
#define FRAGMENT_PAGE_CLOCK 2
#define FRAGMENT_PAGE_CLOCK_SET 1

#define PAGE_COUNT 3 //页面数量

/**
 * 事件类型
 */
#define EVENT_VFD_OPEN BIT(0)
#define EVENT_VFD_CLOSE BIT(1)
#define EVENT_VFD_BRIGHTNESS_DECREASE BIT(2)
#define EVENT_VFD_BRIGHTNESS_INCREASE BIT(3)

#define EVENT_RGB_OPEN BIT(4)
#define EVENT_RGB_CLOSE BIT(5)

#define EVENT_G1_OPEN BIT(6)
#define EVENT_G1_CLOSE BIT(7)

typedef void (*fun_button_click_callback)(u8 btn_key, button_state_t btn_action);

typedef void (*fun_active_on_resume)(void *params);

typedef void (*fun_active_on_pause)(void *params);

typedef void (*fun_active_on_loop)(void *params);

typedef void (*fun_active_on_create)(void *params);

typedef struct {
    u8 fid;  // 页面标识ID
    u8 active;
    fun_button_click_callback btn_callback;
    fun_active_on_create on_create;
    fun_active_on_resume on_resume;
    fun_active_on_pause on_pause;
    fun_active_on_loop on_loop;
} fragmen_obj;

extern EventGroupHandle_t fragment_event_handle;
extern rx8025_timeinfo timeinfo;

void fragment_init();

void fragment_loop();

void replace_page(u8 fid, void *params);

#endif