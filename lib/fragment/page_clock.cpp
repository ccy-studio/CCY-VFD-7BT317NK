/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:48:11
 * @LastEditTime: 2023-09-20 18:02:45
 */
#include "fragment.h"

thread_obj* thread_clock;

rx8025_timeinfo timeinfo;
u8 time_colon_show = 0;  // 时钟冒号显示状态

static void click_callback(u8 btn_key, u8 btn_action) {
    switch (btn_action) {
        case BUTTON_ACTION_PRESS_DOWN:
            if (btn_key == KEY1) {
                // 亮度减
                send_global_event(EVENT_VFD_BRIGHTNESS_DECREASE);
            } else if (btn_key == KEY2) {
                // 亮度加
                send_global_event(EVENT_VFD_BRIGHTNESS_INCREASE);
            } else if (btn_key == KEY3) {
            }
            break;
        case BUTTON_ACTION_PRESS_UP:
            /* code */
            break;
        case BUTTON_ACTION_PRESS_LONG:
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
    rx8025_read_all(&timeinfo);
    vfd_gui_set_text(formart_time(&timeinfo).c_str());
    vfd_gui_set_icon(ICON_CLOCK, 1);
    vfd_gui_set_maohao1(time_colon_show);
    vfd_gui_set_maohao2(time_colon_show);
    time_colon_show = !time_colon_show;
}

static void on_create(void* params) {
    thread_clock = thread_create(TID_CLOCK, VFD_TIME_FRAME, thread_run_clock);
}

static void on_resume(void* params) {
    thread_start(TID_CLOCK);
}

static void on_pause(void* params) {
    thread_stop(TID_CLOCK);
}

static void on_loop(void* params) {}

static void on_event(u8 event_id, void* params) {}

fragmen_obj page_clock = {.fid = FRAGMENT_PAGE_CLOCK,
                          .active = 0,
                          .btn_callback = click_callback,
                          .on_create = on_create,
                          .on_resume = on_resume,
                          .on_pause = on_pause,
                          .on_loop = on_loop,
                          .on_event = on_event};