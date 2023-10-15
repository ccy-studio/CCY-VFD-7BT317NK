/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-10 17:55:53
 * @LastEditTime: 2023-10-12 15:19:04
 */
#ifndef __COMPONENTS_BUTTON_H__
#define __COMPONENTS_BUTTON_H__

#include <constant.h>

#define BTN_MAX 5
#define BTN_PRESS_LEVEL 0
#define BTN_SCAN_MS 50
#define BTN_LONG_PRESS_MS 2000
#define BTN_LONG_DOUBLE_MS 500

/**
 * Button states/events
 */
typedef enum {
    BUTTON_PRESSED = 0,
    BUTTON_RELEASED,
    BUTTON_PRESSED_LONG,
    BUTTON_DOUBLE_PRESS,
} button_state_t;

typedef struct {
    u8 gpio;
    button_state_t state;
    struct {
        u32 press_time;
        u8 d_count;
        u32 d_time;
    } internal;
} button_t;

extern QueueHandle_t btn_queue;

void button_init();
void button_add_btn(button_t* button);
void button_remove_btn(button_t* button);

#endif /* __COMPONENTS_BUTTON_H__ */