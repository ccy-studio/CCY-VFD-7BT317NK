/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:46:28
 * @LastEditTime: 2023-09-24 00:37:50
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

typedef void (*rx8025_read_done_fun)(void);

/**
 * 初始化
 */
void rx8025t_init(rx8025_read_done_fun fun);

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
String formart_time(rx8025_timeinfo* timeinfo);
/**
 * 格式日期 YYMMdd
 */
String formart_date(rx8025_timeinfo* timeinfo);

#endif