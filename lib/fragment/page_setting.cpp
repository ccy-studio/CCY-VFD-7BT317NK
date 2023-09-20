/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-20 11:53:27
 * @LastEditTime: 2023-09-20 17:17:35
 */
#include "fragment.h"

static void click_callback(u8 btn_key, u8 btn_action) {}

static void on_create(void* params) {}

static void on_resume(void* params) {}

static void on_pause(void* params) {}

static void on_loop(void* params) {}
static void on_event(u8 event_id, void* params) {}

fragmen_obj page_setting = {.fid = FRAGMENT_PAGE_SETTING,
                          .active = 0,
                          .btn_callback = click_callback,
                          .on_create = on_create,
                          .on_resume = on_resume,
                          .on_pause = on_pause,
                          .on_loop = on_loop,
                          .on_event = on_event};