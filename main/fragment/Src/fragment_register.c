/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-10 16:13:23
 * @LastEditTime: 2023-10-10 16:18:34
 */
#include "fragment.h"

// 引用
extern const fragmen_obj page_clock;
extern const fragmen_obj page_setting;
extern const fragmen_obj page_clock_set;
// End

fragmen_obj fragment_page_arr[PAGE_COUNT];

void fragment_bind() {
    fragment_page_arr[0] = page_clock;
    fragment_page_arr[1] = page_setting;
    fragment_page_arr[2] = page_clock_set;
}