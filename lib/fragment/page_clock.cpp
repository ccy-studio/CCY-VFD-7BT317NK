/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:48:11
 * @LastEditTime: 2023-09-25 22:07:21
 */
#include "fragment.h"

thread_obj* thread_clock;

static u8 time_colon_show = 0;  // 时钟冒号显示状态

#define CONTENT_DATE 0                  // 显示年份
#define CONTENT_CUSTOM 1                // 显示滚动文字
#define CONTENT_TIME 2                  // 显示时间
static u8 content_type = CONTENT_TIME;  // 显示内容Flag

static void click_callback(u8 btn_key, u8 btn_action) {
    switch (btn_action) {
        case BUTTON_ACTION_PRESS_DOWN:
            if (btn_key == KEY1) {
                // 亮度减
                send_global_event(EVENT_VFD_BRIGHTNESS_DECREASE, NULL);
            } else if (btn_key == KEY2) {
                // 亮度加
                send_global_event(EVENT_VFD_BRIGHTNESS_INCREASE, NULL);
            } else if (btn_key == KEY3) {
                // 切换显示状态
                vfd_gui_cancel_long_text();
                if (content_type + 1 >= 3) {
                    content_type = 0;
                } else {
                    content_type++;
                }
            }
            break;
        case BUTTON_ACTION_DOUBLE_PRESS:
            Serial.println("BUTTON_ACTION_DOUBLE_PRESS");
            if (btn_key == KEY3) {
                // 进入设置页面
                replace_page(FRAGMENT_PAGE_SETTING);
            }
            break;
        default:
            break;
    }
}

void thread_run_clock() {
    if (content_type == CONTENT_CUSTOM) {
        return;
    }
    rtc_update();
    rx8025_timeinfo* timeinfo = rtc_get_timeinfo();
    String str;
    if (CONTENT_TIME == content_type) {
        str = formart_time(timeinfo);
        time_colon_show = !time_colon_show;
    } else if (CONTENT_DATE == content_type) {
        str = formart_date(timeinfo);
        time_colon_show = 0;
    }
    vfd_gui_set_text(str.c_str(), time_colon_show);
    vfd_gui_set_icon(ICON_CLOCK, 1);
}

static void on_create(void* params) {
    thread_clock = thread_create(TID_CLOCK, VFD_TIME_FRAME, thread_run_clock);
}

static void on_resume(void* params) {
    thread_start_obj(thread_clock);
}

static void on_pause(void* params) {
    thread_stop_obj(thread_clock);
    vfd_gui_cancel_long_text();
    vfd_gui_clear();
}

static void on_loop(void* params) {
    if (CONTENT_CUSTOM == content_type) {
        vfd_gui_set_long_text(setting_obj.custom_long_text,
                              setting_obj.custom_long_text_frame, 2);
    }
}

static void on_event(u8 event_id, void* params) {}

fragmen_obj page_clock = {.fid = FRAGMENT_PAGE_CLOCK,
                          .active = 0,
                          .btn_callback = click_callback,
                          .on_create = on_create,
                          .on_resume = on_resume,
                          .on_pause = on_pause,
                          .on_loop = on_loop,
                          .on_event = on_event};