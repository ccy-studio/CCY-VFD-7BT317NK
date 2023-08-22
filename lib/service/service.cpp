/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系
 *[yustart@foxmail.com] 进行授权。
 *
 * 在遵循以下条件的情况下，您可以自由修改、使用和分发本代码：
 * - 您必须保留此版权声明的所有内容。
 * - 您不得用于任何违法、滥用、恶意的活动。
 * - 您的使用不应损害任何个人、组织或作者的声誉。
 *
 * 作者不承担因使用本代码而产生的任何担保责任。作者对因使用本代码所导致的任何直接或间接损失不承担责任。
 * Github: https://github.com/ccy-studio/CCY-VFD-7BT317NK
 ***********************************************************************************************/

/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-21 15:47:38
 * @LastEditTime: 2023-08-22 15:32:27
 * 结构体用指针传递更节省内存空间
 */
#include <service.h>
#include <store.h>

static char countdown_save_time[9];

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

void logic_handler_alarm_clock(tm* timeinfo, AlarmClockCallback callback) {
    if (!setting_obj.alarm_clock) {
        return;
    }
    // 判断生效条件 今日是否生效
    if (!is_number_in_array(setting_obj.alarm_clock_enable_days, 7,
                            timeinfo->tm_wday)) {
        return;
    }
    if (strlen(setting_obj.alarm_clock_time) == 0) {
        return;
    }
    static char save_alarm_time[9];
    static u8 alarm_hours, alarm_minutes, alarm_seconds;
    if (strcmp(save_alarm_time, setting_obj.alarm_clock_time)) {
        // 需要更新
        memcpy(save_alarm_time, setting_obj.alarm_clock_time,
               sizeof(save_alarm_time));
        sscanf(save_alarm_time, "%hhd:%hhd:%hhd", &alarm_hours, &alarm_minutes,
               &alarm_seconds);
    }
    if (timeinfo->tm_hour == alarm_hours && timeinfo->tm_min == alarm_minutes &&
        timeinfo->tm_sec == alarm_seconds) {
        // setup 闹钟时间到了
        callback(1);
        return;
    }
}

void logic_handler_countdown(tm* timeinfo, CountdownCallback callback) {
    if (!setting_obj.countdown) {
        return;
    }
    if (strlen(setting_obj.countdown_time) == 0) {
        return;
    }

    static u8 s;
    static u8 count_hours, count_minutes, count_seconds;
    if (strcmp(countdown_save_time, setting_obj.countdown_time)) {
        memcpy(countdown_save_time, setting_obj.countdown_time,
               sizeof(countdown_save_time));
        sscanf(countdown_save_time, "%hhd:%hhd:%hhd", &count_hours,
               &count_minutes, &count_seconds);
        if (count_seconds == 0 && count_minutes == 0 && count_hours == 0) {
            logic_handler_countdown_stop();
            return;
        }
        s = timeinfo->tm_sec;
        callback(1, count_hours, count_minutes, count_seconds);
    } else {
        u8 is_change = 0;
        // 非首次
        if (count_seconds == 0 && count_minutes == 0 && count_hours == 0) {
            // 结束
            logic_handler_countdown_stop();
            callback(0, count_hours, count_minutes, count_seconds);
            return;
        } else {
            // check sec
            if (s != timeinfo->tm_sec) {
                count_seconds--;
                is_change = 1;
            }
            if (count_seconds == 255) {
                if (count_hours != 0 || count_minutes != 0) {
                    count_seconds = 59;
                    count_minutes--;
                    if (count_minutes == 255) {
                        if (count_hours != 0) {
                            count_minutes = 59;
                            count_hours--;
                            if (count_hours == 255) {
                                count_hours = 0;
                            }
                        } else {
                            count_minutes = 0;
                        }
                    }
                } else {
                    count_seconds = 0;
                }
            }
        }
        if (is_change) {
            s = timeinfo->tm_sec;
            callback(1, count_hours, count_minutes, count_seconds);
        }
    }
}

void logic_handler_countdown_stop() {
    if (!setting_obj.countdown) {
        return;
    }
    memset(countdown_save_time, 0, sizeof(countdown_save_time));
    memset(setting_obj.countdown_time, 0, sizeof(setting_obj.countdown_time));
    setting_obj.countdown = 0;
    store_save_setting(&setting_obj);
}