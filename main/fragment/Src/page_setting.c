/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:53:27
 * @LastEditTime: 2023-10-10 16:44:45
 */
#include "fragment.h"

typedef struct {
    char title[10];
    u8 open_state;
} setting_content;

#define STR_Y "Y"
#define STR_N "N"

#define CONTENT_SET_WIFI 0
#define CONTENT_SET_RGB 1
#define CONTENT_SET_G1 2
#define CONTENT_SET_CLOCK 3

static u8 content_type = CONTENT_SET_WIFI;

static setting_content content_arr[4] = {{"WIFI:", 0},
                                         {"RGB:",  0},
                                         {"G1:",   0},
                                         {"CLOCK", 0}};
static char content_str[10], cache_str[10];

static void click_callback(u8 btn_key, button_state_t btn_action) {
    switch (btn_action) {
        case BUTTON_PRESSED:
            if (btn_key == KEY1) {
                // 设置切换
                if (content_type == 0) {
                    content_type = 3;
                } else {
                    content_type--;
                }
            } else if (btn_key == KEY2) {
                // 设置切换
                if (content_type + 1 >= 4) {
                    content_type = 0;
                } else {
                    content_type++;
                }
            } else if (btn_key == KEY3) {
                setting_content obj = content_arr[content_type];
                switch (content_type) {
                    case CONTENT_SET_WIFI:
                        obj.open_state = !obj.open_state;
                        printf("WIFI\n");
                        break;
                    case CONTENT_SET_RGB:
                        obj.open_state = !obj.open_state;
                        setting_obj.rgb_open = obj.open_state;
                        store_save_setting(&setting_obj);
                        break;
                    case CONTENT_SET_G1:
                        obj.open_state = !obj.open_state;
                        setting_obj.anno_open = obj.open_state;
                        store_save_setting(&setting_obj);
                        break;
                    case CONTENT_SET_CLOCK:
                        replace_page(FRAGMENT_PAGE_CLOCK_SET, NULL);
                        break;
                    default:
                        break;
                }
            }
            break;
        case BUTTON_PRESSED_LONG:
            if (btn_key == KEY3) {
                // 退出设置
                replace_page(FRAGMENT_PAGE_CLOCK, NULL);
            }
            break;
        default:
            break;
    }
}

static void on_create(void *params) {}

static void on_resume(void *params) {
    // 首次进入更新设置
    content_arr[CONTENT_SET_WIFI].open_state = 0;  // WIFI开关
    content_arr[CONTENT_SET_RGB].open_state = setting_obj.rgb_open;
    content_arr[CONTENT_SET_G1].open_state = setting_obj.anno_open;
    content_type = CONTENT_SET_WIFI;
}

static void on_pause(void *params) {
    vfd_gui_cancel_long_text();
    vfd_gui_clear();
}

static void on_loop(void *params) {
    delay_ms(500);
    memset(content_str, 0, sizeof(content_str));
    setting_content obj = content_arr[content_type];
    strcat(content_str, (const char *) obj.title);
    strcat(content_str, obj.open_state ? STR_Y : STR_N);
//    if (!strcmp(content_str, cache_str)) {
//        return;
//    }
    vfd_gui_set_text(content_str, 0);
//    memset(cache_str, 0, sizeof(cache_str));
//    strcpy(cache_str, content_str);
}

const fragmen_obj page_setting = {.fid = FRAGMENT_PAGE_SETTING,
        .active = 0,
        .btn_callback = click_callback,
        .on_create = on_create,
        .on_resume = on_resume,
        .on_pause = on_pause,
        .on_loop = on_loop};