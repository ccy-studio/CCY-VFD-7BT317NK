/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:53:27
 * @LastEditTime: 2023-09-25 11:38:11
 */
#include "fragment.h"

typedef struct {
    char title[10];
    u8 open_state = 0;
} setting_content;

#define STR_Y 'Y'
#define STR_N 'N'

#define CONTENT_SET_WIFI 0
#define CONTENT_SET_RGB 1
#define CONTENT_SET_G1 2
#define CONTENT_SET_CLOCK 3

static u8 content_type = CONTENT_SET_WIFI;

static setting_content content_arr[4] = {{"WIFI:"},
                                         {"RGB:"},
                                         {"G1:"},
                                         {"CLOCK"}};
static String content_str;

static void click_callback(u8 btn_key, u8 btn_action) {
    switch (btn_action) {
        case BUTTON_ACTION_PRESS_DOWN:
            if (btn_key == KEY1) {
                // 设置切换
                content_type = content_type <= 0 ? 0 : (content_type - 1) % 4;
            } else if (btn_key == KEY2) {
                // 设置切换
                content_type = (content_type + 1) % 4;
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
                        replace_page(FRAGMENT_PAGE_CLOCK_SET);
                        break;
                    default:
                        break;
                }
            }
            break;
        case BUTTON_ACTION_DOUBLE_PRESS:
            if (btn_key == KEY3) {
                // 退出设置
                replace_page(FRAGMENT_PAGE_CLOCK);
            }
            break;
        default:
            break;
    }
}

static void on_create(void* params) {}

static void on_resume(void* params) {
    vfd_gui_clear();
    // 首次进入更新设置
    content_arr[CONTENT_SET_WIFI].open_state = 0;  // WIFI开关
    content_arr[CONTENT_SET_RGB].open_state = setting_obj.rgb_open;
    content_arr[CONTENT_SET_G1].open_state = setting_obj.anno_open;
    content_type = CONTENT_SET_WIFI;
}

static void on_pause(void* params) {
    vfd_gui_cancel_long_text();
    vfd_gui_clear();
}

static void on_loop(void* params) {
    content_str.clear();
    setting_content obj = content_arr[content_type];
    content_str += obj.title;
    content_type += obj.open_state ? STR_Y : STR_N;
    vfd_gui_set_text(content_str.c_str());
}
static void on_event(u8 event_id, void* params) {}

fragmen_obj page_setting = {.fid = FRAGMENT_PAGE_SETTING,
                            .active = 0,
                            .btn_callback = click_callback,
                            .on_create = on_create,
                            .on_resume = on_resume,
                            .on_pause = on_pause,
                            .on_loop = on_loop,
                            .on_event = on_event};