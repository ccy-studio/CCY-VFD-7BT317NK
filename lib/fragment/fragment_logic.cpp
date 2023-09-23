/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 14:46:13
 * @LastEditTime: 2023-09-24 00:27:59
 */
#include "fragment.h"

thread_obj* thread_rgb = NULL;  // RGB 线程句柄
thread_obj* thread_g1 = NULL;   // G1动画 线程句柄

/**
 * VFD操作 0x1?
 */
void handler_vfd_controll(u8 event_id, void* params) {
    static u8 vfd_brightness = 7;
    switch (event_id) {
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
void thread_run_rgb() {
    rgb_set_style(setting_obj.rgb_style);
    rgb_set_brightness(setting_obj.rgb_brightness);
    rbg_frame_update();
}

void handler_rgb_controll(u8 event_id) {
    if (event_id == EVENT_RGB_OPEN) {
        if (thread_rgb == NULL) {
            // 创建线程
            rgb_setup();
            thread_rgb = thread_create(TID_RGB, RGB_ANNO_FRAME, thread_run_rgb);
        }
        thread_start(TID_RGB);
    } else if (event_id == EVENT_RGB_CLOSE) {
        thread_stop(TID_RGB);
        rgb_clear();
    }
}

/**
 * G1动画执行线程
 */
void thread_run_g1() {
    vfd_gui_anno_for_g1(0);
}
void handelr_g1_anno_controll(u8 event_id) {
    if (event_id == EVENT_G1_OPEN) {
        if (thread_g1 == NULL) {
            // 创建线程
            thread_rgb = thread_create(TID_G1, G1_ANNO_FRAME, thread_run_g1);
        }
        thread_start(TID_G1);
    } else if (event_id == EVENT_G1_CLOSE) {
        thread_stop(TID_G1);
    }
}

/**
 * 实时参数检查
 */
void configuration_check() {
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