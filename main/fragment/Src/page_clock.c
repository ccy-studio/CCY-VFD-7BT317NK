/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:48:11
 * @LastEditTime: 2023-10-10 16:44:20
 */
#include "fragment.h"

static u8 time_colon_show = 0;  // 时钟冒号显示状态

#define CONTENT_DATE 0                  // 显示年份
#define CONTENT_CUSTOM 1                // 显示滚动文字
#define CONTENT_TIME 2                  // 显示时间
static u8 content_type = CONTENT_TIME;  // 显示内容Flag

static char vfd_content_buf[10];

static void click_callback(u8 btn_key, button_state_t btn_action) {
    switch (btn_action) {
        case BUTTON_PRESSED:
            if (btn_key == KEY1) {
                // 亮度减
                xEventGroupSetBits(fragment_envent_handle,
                                   EVENT_VFD_BRIGHTNESS_DECREASE);
            } else if (btn_key == KEY2) {
                // 亮度加
                xEventGroupSetBits(fragment_envent_handle,
                                   EVENT_VFD_BRIGHTNESS_INCREASE);
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
        case BUTTON_CLICKED:
            printf("BUTTON_ACTION_DOUBLE_PRESS");
            if (btn_key == KEY3) {
                // 进入设置页面
                replace_page(FRAGMENT_PAGE_SETTING, NULL);
            }
            break;
        default:
            break;
    }
}

static void on_create(void* params) {}

static void on_resume(void* params) {}

static void on_pause(void* params) {
    vfd_gui_cancel_long_text();
    vfd_gui_clear();
}

static void on_loop(void* params) {
    if (CONTENT_CUSTOM == content_type) {
        vfd_gui_set_long_text(setting_obj.custom_long_text,
                              setting_obj.custom_long_text_frame, 2);
    } else {
        if (CONTENT_TIME == content_type) {
            formart_time(&timeinfo, vfd_content_buf, sizeof(vfd_content_buf));
            time_colon_show = !time_colon_show;
        } else if (CONTENT_DATE == content_type) {
            formart_date(&timeinfo, vfd_content_buf, sizeof(vfd_content_buf));
            time_colon_show = 0;
        }
        vfd_gui_set_text(vfd_content_buf, time_colon_show);
        vfd_gui_set_icon(ICON_CLOCK, 1);
        delay_ms(VFD_TIME_FRAME);
    }
}

const fragmen_obj page_clock = {.fid = FRAGMENT_PAGE_CLOCK,
                                .active = 0,
                                .btn_callback = click_callback,
                                .on_create = on_create,
                                .on_resume = on_resume,
                                .on_pause = on_pause,
                                .on_loop = on_loop};