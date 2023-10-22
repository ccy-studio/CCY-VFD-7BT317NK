/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 14:46:13
 * @LastEditTime: 2023-10-10 16:26:44
 */
#include "fragment.h"
#include "smart_wifi.h"

static TaskHandle_t task_rgb = NULL;
static TaskHandle_t task_g1 = NULL;
static TaskHandle_t task_rx8025 = NULL;

rx8025_timeinfo timeinfo;

volatile bool power_state = true; //开关机状态

/**
 * VFD操作 0x1?
 */
void handler_vfd_control() {
    EventBits_t r_event = xEventGroupWaitBits(
            fragment_event_handle,
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
void thread_run_rgb(void *params) {
    while (1) {
        rgb_fun_set_style(glob_setting_config.rgb_style);
        rgb_fun_set_brightness(glob_setting_config.rgb_brightness);
        rgb_fun_anno_update();
        delay_ms(RGB_ANNO_FRAME);
    }
}

void handler_rgb_control(u8 event_id) {
    if (event_id == EVENT_RGB_OPEN) {
        if (task_rgb == NULL) {
            // 创建线程
            xTaskCreate(thread_run_rgb, "RGB", 1024, NULL, 1, &task_rgb);
        }
    } else if (event_id == EVENT_RGB_CLOSE) {
        if (task_rgb != NULL) {
            vTaskDelete(task_rgb);
            task_rgb = NULL;
            rgb_clear();
        }
    }
}

/**
 * G1动画执行线程
 */
void thread_run_g1(void *params) {
    while (1) {
        vfd_gui_anno_for_g1();
        delay_ms(G1_ANNO_FRAME);
    }
}

void handle_g1_event(u8 event_id) {
    if (event_id == EVENT_G1_OPEN) {
        if (task_g1 == NULL) {
            // 创建线程
            xTaskCreate(thread_run_g1, "G1", 1024, NULL, 1, &task_g1);
        }
    } else if (event_id == EVENT_G1_CLOSE) {
        if (task_g1 != NULL) {
            vTaskDelete(task_g1);
            task_g1 = NULL;
            delay_ms(50);
        }
    }
}

/**
 * 实时参数检查
 */
static void configuration_check() {
    if (!power_state) {
        return;
    }
    store_setting_obj *obj = get_store();
    if (obj->rgb_open) {
        handler_rgb_control(EVENT_RGB_OPEN);
    } else {
        handler_rgb_control(EVENT_RGB_CLOSE);
    }

    if (obj->anno_open) {
        handle_g1_event(EVENT_G1_OPEN);
    } else {
        handle_g1_event(EVENT_G1_CLOSE);
    }
}

static void rx8025_update_fun(void *params) {
    u32 icon = ICON_CLOCK;
    while (1) {
        rx8025_time_get(&timeinfo);
        //检查ICON图标信息
        if (wifi_get_connect_state() == WIFI_CONNECTED) {
            icon = ICON_CLOCK | ICON_WIFI;
        } else {
            icon = ICON_CLOCK;
        }
        vfd_set_save_icon(icon);
        //判断是否开启了G1动画，如果没有开启则需要手动更新一次到PT6315 Ram中
        if (!glob_setting_config.anno_open) {
            vfd_gui_set_icon(icon);
        }
        delay_ms(500);
    }
}

void auto_power_handle(u8 state) {
    if (state && !power_state) {
        power_state = true;
        //开机
        xEventGroupSetBits(fragment_event_handle, EVENT_VFD_OPEN);
    } else if (!state) {
        //关机
        power_state = false;
        xEventGroupSetBits(fragment_event_handle, EVENT_VFD_CLOSE);
        handler_rgb_control(EVENT_RGB_CLOSE);
        handle_g1_event(EVENT_G1_CLOSE);
        wifi_unconnect();
    }
}


static void alarm_clock_handle(u8 state) {
    if (state) {
        //闹钟时间到
        for (size_t i = 0; i < 5; i++) {
            buzzer_fast_play(500);
            delay_us(500);
        }
    }
}

static void service_task_fun(void *params) {
    while (1) {
        configuration_check();  // 检查Store参数的变化
        handler_vfd_control();
        logic_handler_auto_power(&timeinfo, auto_power_handle);
        logic_handler_alarm_clock(&timeinfo, alarm_clock_handle);
        delay_ms(1);
    }
}

void fragment_service_init() {
    xTaskCreate(service_task_fun, "Ser", 2048, NULL, 1, NULL);
    xTaskCreate(rx8025_update_fun, "TIME", 1024, NULL, 100, &task_rx8025);
}