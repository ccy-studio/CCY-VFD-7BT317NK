/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 14:46:13
 * @LastEditTime: 2023-10-10 16:26:44
 */
#include "fragment.h"

static TaskHandle_t task_rgb = NULL;
static TaskHandle_t task_g1 = NULL;
static TaskHandle_t task_rx8025 = NULL;

rx8025_timeinfo timeinfo;

/**
 * VFD操作 0x1?
 */
void handler_vfd_controll() {
    EventBits_t r_event = xEventGroupWaitBits(
        fragment_envent_handle,
        EVENT_VFD_OPEN | EVENT_VFD_CLOSE | EVENT_VFD_BRIGHTNESS_INCREASE |
            EVENT_VFD_BRIGHTNESS_DECREASE,
        pdTRUE, pdFALSE, 0);

    static u8 vfd_brightness = 7;
    switch (r_event) {
        case EVENT_VFD_OPEN:
            vfd_gui_init();
            break;
        case EVENT_VFD_CLOSE:
            vfd_gui_cancel_long_text();
            vfd_gui_stop();
            break;
        case EVENT_VFD_BRIGHTNESS_INCREASE:  // 亮度加
            vfd_brightness = vfd_brightness + 3 > 7 ? 7 : vfd_brightness + 3;
            vfd_gui_set_blk_level(vfd_brightness);
            break;
        case EVENT_VFD_BRIGHTNESS_DECREASE:  // 亮度减少
            vfd_brightness = vfd_brightness - 3 < 1 ? 1 : vfd_brightness - 3;
            vfd_gui_set_blk_level(vfd_brightness);
            break;
        default:
            break;
    }
}

/**
 * RGB执行线程
 */
void thread_run_rgb(void* params) {
    while (1) {
        rgb_fun_set_style(setting_obj.rgb_style);
        rgb_fun_set_brightness(setting_obj.rgb_brightness);
        rgb_fun_anno_update();
        delay_ms(RGB_ANNO_FRAME);
    }
}

void handler_rgb_controll(u8 event_id) {
    if (event_id == EVENT_RGB_OPEN) {
        if (task_rgb == NULL) {
            // 创建线程
            xTaskCreate(thread_run_rgb, "RGB", 1024, NULL, 5, &task_rgb);
        }
    } else if (event_id == EVENT_RGB_CLOSE) {
        if (task_rgb != NULL) {
            vTaskDelete(&task_rgb);
            task_rgb = NULL;
        }
        rgb_clear();
    }
}

/**
 * G1动画执行线程
 */
void thread_run_g1(void* params) {
    while (1) {
        vfd_gui_anno_for_g1();
        delay_ms(RGB_ANNO_FRAME);
    }
}
void handelr_g1_anno_controll(u8 event_id) {
    if (event_id == EVENT_G1_OPEN) {
        if (task_g1 == NULL) {
            // 创建线程
            xTaskCreate(thread_run_g1, "G1", 1024, NULL, 5, &task_g1);
        }
    } else if (event_id == EVENT_G1_CLOSE) {
        if (task_g1 != NULL) {
            vTaskDelete(&task_g1);
            task_g1 = NULL;
        }
    }
}

/**
 * 实时参数检查
 */
static void configuration_check() {
    if (setting_obj.rgb_open) {
        handler_rgb_controll(EVENT_RGB_OPEN);
    } else {
        handler_rgb_controll(EVENT_RGB_CLOSE);
    }

    if (setting_obj.anno_open) {
        handelr_g1_anno_controll(EVENT_G1_OPEN);
    } else {
        handelr_g1_anno_controll(EVENT_G1_CLOSE);
    }
}

static void rx8025_update_fun(void* params) {
    while (1) {
        rx8025_time_get(&timeinfo);
        delay_ms(500);
    }
}

static void service_task_fun(void* params) {
    while (1) {
        configuration_check();  // 检查Store参数的变化
        handler_vfd_controll();
        delay_ms(1);
    }
}

void fragment_servicce_init() {
    xTaskCreate(service_task_fun, "FS", 2048, NULL, 1, NULL);
    xTaskCreate(rx8025_update_fun, "TIME", 1024, NULL, 2, &task_rx8025);
}