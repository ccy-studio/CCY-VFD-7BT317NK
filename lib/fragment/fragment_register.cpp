#include "fragment.h"

// 引用
extern fragmen_obj page_clock;
extern fragmen_obj page_setting;
extern fragmen_obj page_clock_set;
// End

fragmen_obj fragment_page_arr[PAGE_COUNT] = {page_clock, page_setting,
                                             page_clock_set};