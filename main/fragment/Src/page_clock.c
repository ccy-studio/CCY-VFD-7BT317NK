/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:48:11
 * @LastEditTime: 2023-10-10 16:44:20
 */
#include "fragment.h"
#include "smart_wifi.h"

static u8 time_colon_show = 0;  // 时钟冒号显示状态

#define CONTENT_TIME 0                  // 显示时间
#define CONTENT_DATE 1                  // 显示年份
#define CONTENT_CUSTOM 2                // 显示滚动文字
#define CONTENT_IP 3                  // 显示IP
#define  CONTENT_SIZE 4 //内容类型总数量
static u8 content_type = CONTENT_TIME;  // 显示内容Flag

static char vfd_content_buf[10];
static TaskHandle_t vfd_thread = NULL;

static void click_callback(u8 btn_key, button_state_t btn_action) {
    switch (btn_action) {
        case BUTTON_RELEASED:
            if (btn_key == KEY1) {
                // 亮度减
                xEventGroupSetBits(fragment_event_handle,
                                   EVENT_VFD_BRIGHTNESS_DECREASE);
            } else if (btn_key == KEY2) {
                // 亮度加
                xEventGroupSetBits(fragment_event_handle,
                                   EVENT_VFD_BRIGHTNESS_INCREASE);
            } else if (btn_key == KEY3) {
                // 切换显示状态
                vfd_gui_cancel_long_text();
                if (content_type + 1 >= CONTENT_SIZE) {
                    content_type = 0;
                } else {
                    content_type++;
                }
            }
            break;
        case BUTTON_PRESSED_LONG:
            ESP_LOGI(APP_TAG, "BUTTON_PRESSED_LONG长按\n");
            if (btn_key == KEY3) {
                // 进入设置页面
                replace_page(FRAGMENT_PAGE_SETTING, NULL);
            }
            break;
        default:
            break;
    }
}

static void vfd_run_fun(void *params) {
    while (1) {
        if (CONTENT_CUSTOM == content_type) {
            vfd_gui_set_long_text(glob_setting_config.custom_long_text,
                                  glob_setting_config.custom_long_text_frame, 2);
        } else if (CONTENT_IP == content_type) {
            static char ip_buf[20];
            memset(ip_buf, 0, sizeof(ip_buf));
            if (wifi_get_connect_state() == WIFI_CONNECTED) {
                strcat(ip_buf, wifi_get_ip());
            } else {
                //UN connect
                strcat(ip_buf, "WIFI UN-CONNECT");
            }
            vfd_gui_set_long_text(ip_buf, 150, 2);
        } else {
            if (CONTENT_TIME == content_type) {
                formart_time(&timeinfo, vfd_content_buf, sizeof(vfd_content_buf));
                time_colon_show = !time_colon_show;
            } else if (CONTENT_DATE == content_type) {
                formart_date(&timeinfo, vfd_content_buf, sizeof(vfd_content_buf));
                time_colon_show = 1;
            }
            vfd_gui_set_text(vfd_content_buf, time_colon_show);
            delay_ms(VFD_TIME_FRAME);
        }
    }
}

static void on_create(void *params) {}

static void on_resume(void *params) {
    xTaskCreate(vfd_run_fun, "VFD", 4096, NULL, 1, &vfd_thread);
}

static void on_pause(void *params) {
    vTaskDelete(vfd_thread);
    vfd_thread = NULL;
    vfd_gui_cancel_long_text();
    vfd_gui_clear();
}

static void on_loop(void *params) {

}

const fragmen_obj page_clock = {.fid = FRAGMENT_PAGE_CLOCK,
        .active = 0,
        .btn_callback = click_callback,
        .on_create = on_create,
        .on_resume = on_resume,
        .on_pause = on_pause,
        .on_loop = on_loop};