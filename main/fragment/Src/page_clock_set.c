/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:48:11
 * @LastEditTime: 2023-10-10 16:41:22
 */
#include "fragment.h"

typedef struct {
    u8 id;
    u8 val;
    int min;
    int max;
    char title[4];
} content_show_obj;

static content_show_obj page_obj[6] = {
        {0, 23, 23, 255, "N20"},
        {1, 1,  1,  12,  "Y:"},
        {2, 1,  1,  31,  "R:"},
        {3, 0,  0,  24,  "H:"},
        {4, 0,  0,  59,  "M:"},
        {5, 0,  0,  59,  "S:"},
};

static u8 content_type = 0;  // 显示内容Flag
static char content_buf[10];
static u8 save_and_back = 0;

static void set_content();

static void click_callback(u8 btn_key, button_state_t btn_action) {
    content_show_obj obj = page_obj[content_type];
    switch (btn_action) {
        case BUTTON_RELEASED:
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
            set_content();
            break;
        case BUTTON_PRESSED_LONG:
            if (btn_key == KEY3) {
                save_and_back = 1;
            }
            break;
        default:
            break;
    }
}

static void on_create(void *params) {}

static void on_resume(void *params) {
    // 进入设置页面
    page_obj[0].val = timeinfo.year;
    page_obj[1].val = timeinfo.month;
    page_obj[2].val = timeinfo.day;
    page_obj[3].val = timeinfo.hour;
    page_obj[4].val = timeinfo.min;
    page_obj[5].val = timeinfo.sec;
    content_type = 0;
    save_and_back = 0;
    set_content();
}

static void on_pause(void *params) {
    vfd_gui_cancel_long_text();
    vfd_gui_clear();
}

static void set_content() {
    memset(content_buf, 0, sizeof(content_buf));
    content_show_obj obj = page_obj[content_type];
    strcat(content_buf, (const char *) &obj.title);
    strcat(content_buf, (const char *) &obj.val);
    vfd_gui_set_text((const char *) content_buf, 0);
}

static void on_loop(void *params) {
    if (save_and_back) {
        // 保存设置
        rx8025_set_time(page_obj[0].val, page_obj[1].val, page_obj[2].val, 1,
                        page_obj[3].val, page_obj[4].val, page_obj[5].val);
        // 进入设置页面
        replace_page(FRAGMENT_PAGE_SETTING, NULL);
        save_and_back = 0;
        return;
    }
}

const fragmen_obj page_clock_set = {.fid = FRAGMENT_PAGE_CLOCK_SET,
        .active = 0,
        .btn_callback = click_callback,
        .on_create = on_create,
        .on_resume = on_resume,
        .on_pause = on_pause,
        .on_loop = on_loop};