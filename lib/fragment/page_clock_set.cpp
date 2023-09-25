/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:48:11
 * @LastEditTime: 2023-09-25 14:41:04
 */
#include "fragment.h"

typedef struct {
    u8 id;
    u8 val;
    u8 min;
    u8 max;
    char title[4];
} content_show_obj;

content_show_obj page_obj[6] = {
    {0, 23, 23, 255, "N20"}, {1, 1, 1, 12, "Y:"}, {2, 1, 1, 31, "R:"},
    {3, 0, 0, 24, "H:"},     {4, 0, 0, 59, "M:"}, {5, 0, 0, 59, "S:"},
};

u8 content_type = 0;  // 显示内容Flag
String content_buf;
u8 save_and_back = 0;

static void click_callback(u8 btn_key, u8 btn_action) {
    content_show_obj obj = page_obj[content_type];
    switch (btn_action) {
        case BUTTON_ACTION_PRESS_DOWN:
            if (btn_key == KEY1) {
                if (obj.val - 1 >= obj.min) {
                    obj.val--;
                }
            } else if (btn_key == KEY2) {
                if (obj.val + 1 <= obj.max) {
                    obj.val++;
                }
            } else if (btn_key == KEY3) {
                if (content_type + 1 >= 6) {
                    content_type = 0;
                } else {
                    content_type++;
                }
            }
            break;
        case BUTTON_ACTION_DOUBLE_PRESS:
            if (btn_key == KEY3) {
                save_and_back = 1;
            }
            break;
        default:
            break;
    }
}

static void on_create(void* params) {}

static void on_resume(void* params) {
    // 进入设置页面
    rx8025_timeinfo* timeinfo = rtc_get_timeinfo();
    page_obj[0].val = timeinfo->year;
    page_obj[1].val = timeinfo->month;
    page_obj[2].val = timeinfo->day;
    page_obj[3].val = timeinfo->hour;
    page_obj[4].val = timeinfo->min;
    page_obj[5].val = timeinfo->sec;
    content_type = 0;
    save_and_back = 0;
}

static void on_pause(void* params) {
    vfd_gui_cancel_long_text();
    vfd_gui_clear();
}

static void on_loop(void* params) {
    if (save_and_back) {
        delay(1);
        // 保存设置
        rx8025_set_time(page_obj[0].val, page_obj[1].val, page_obj[2].val, 1,
                        page_obj[3].val, page_obj[4].val, page_obj[5].val);
        // 进入设置页面
        replace_page(FRAGMENT_PAGE_SETTING);
        save_and_back = 0;
        return;
    }
    delay(10);
    content_buf.clear();
    content_show_obj obj = page_obj[content_type];
    content_buf += obj.title;
    content_buf += obj.val;
    vfd_gui_set_text(content_buf.c_str());
}

static void on_event(u8 event_id, void* params) {}

fragmen_obj page_clock_set = {.fid = FRAGMENT_PAGE_CLOCK_SET,
                              .active = 0,
                              .btn_callback = click_callback,
                              .on_create = on_create,
                              .on_resume = on_resume,
                              .on_pause = on_pause,
                              .on_loop = on_loop,
                              .on_event = on_event};