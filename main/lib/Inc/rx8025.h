/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:46:28
 * @LastEditTime: 2023-10-10 15:40:52
 */
#ifndef __RX8025_H
#define __RX8025_H

#include <constant.h>

#define RX8025_ADDR 0x32

typedef struct {
    u8 year;
    u8 month;
    u8 day;
    u8 week;
    u8 hour;
    u8 min;
    u8 sec;
} rx8025_timeinfo;

/**
 * 初始化
 */
void rx8025t_init(fun_key_action fun);

/**
 * 重置
 */
void rx8025_reset();

/**
 * 设置时间
 */
void rx8025_set_time(u8 year,
                     u8 month,
                     u8 day,
                     u8 week,
                     u8 hour,
                     u8 min,
                     u8 sec);

/**
 * 读取时间
 */
void rx8025_time_get(rx8025_timeinfo* timeinfo);

/**
 * 格式化时间: HHmmss
 */
void formart_time(rx8025_timeinfo* timeinfo, char* buf, size_t buf_size);
/**
 * 格式日期 YYMMdd
 */
void formart_date(rx8025_timeinfo* timeinfo, char* buf, size_t buf_size);

#endif