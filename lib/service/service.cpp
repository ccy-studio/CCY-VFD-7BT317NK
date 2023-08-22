/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-21 15:47:38
 * @LastEditTime: 2023-08-22 10:09:45
 * 结构体用指针传递更节省内存空间
 */
#include <service.h>
#include <store.h>

u8 is_number_in_array(const u8* array, size_t array_size, u8 target) {
    for (size_t i = 0; i < array_size; i++) {
        if (array[i] == target) {
            return 1;  // Number is found
        }
    }
    return 0;  // Number is not found
}

void logic_handler_auto_power(tm* timeinfo, AutoPowerCallback callback) {
    if (setting_obj.auto_power) {
        // 开启配置
        // 判断生效条件
        // printf("今天星期:%d\n", timeinfo->tm_wday);
        if (is_number_in_array(setting_obj.auto_power_enable_days, 7,
                               timeinfo->tm_wday)) {
            // 生效条件满足
            // 1. 判断开机条件 ->优先开机匹配，匹配成功则直接return
            if (strlen(setting_obj.auto_power_open_time)) {
                static char save_open_time[9];
                static u8 open_hours, open_minutes, open_seconds;
                if (strcmp(save_open_time, setting_obj.auto_power_open_time)) {
                    // 不同需要更新
                    memcpy(save_open_time, setting_obj.auto_power_open_time,
                           sizeof(save_open_time));
                    sscanf(save_open_time, "%hhd:%hhd:%hhd", &open_hours,
                           &open_minutes, &open_seconds);
                }
                if (timeinfo->tm_hour == open_hours &&
                    timeinfo->tm_min == open_minutes &&
                    timeinfo->tm_sec == open_seconds) {
                    // setup 开机
                    callback(1);
                    return;
                }
            }

            // 2. 判断关机条件
            if (strlen(setting_obj.auto_power_close_time)) {
                static char save_close_time[9];
                static u8 close_hours, close_minutes, close_seconds;
                if (strcmp(save_close_time,
                           setting_obj.auto_power_close_time)) {
                    // 不同需要更新
                    memcpy(save_close_time, setting_obj.auto_power_close_time,
                           sizeof(save_close_time));
                    sscanf(save_close_time, "%hhd:%hhd:%hhd", &close_hours,
                           &close_minutes, &close_seconds);
                }
                if (timeinfo->tm_hour == close_hours &&
                    timeinfo->tm_min == close_minutes &&
                    timeinfo->tm_sec == close_seconds) {
                    // stop关机
                    callback(0);
                    return;
                }
            }
        }
    }
}

void logic_handler_alarm_clock(tm* timeinfo) {}

void logic_handler_countdown(tm* timeinfo) {}