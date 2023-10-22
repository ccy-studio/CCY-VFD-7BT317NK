#include <service.h>
#include <store.h>

static char countdown_save_time[9];

u8 is_number_in_array(const u8 *array, size_t array_size, u8 target) {
    if (target == 0) {
        target = 7;
    }
    for (size_t i = 0; i < array_size; i++) {
        if (array[i] == target) {
            return 1;  // Number is found
        }
    }
    return 0;  // Number is not found
}

void logic_handler_auto_power(rx8025_timeinfo *timeinfo, AutoPowerCallback callback) {
    if (glob_setting_config.auto_power) {
        // 开启配置
        // 判断生效条件
        // printf("今天星期:%d\n", timeinfo->tm_wday);
        if (is_number_in_array(glob_setting_config.auto_power_enable_days, 7,
                               timeinfo->week)) {
            // 生效条件满足
            // 1. 判断开机条件 ->优先开机匹配，匹配成功则直接return
            if (strlen(glob_setting_config.auto_power_open_time)) {
                static char save_open_time[9];
                static int open_hours, open_minutes, open_seconds;
                static bool open_is_call = false;
                if (strcmp(save_open_time, glob_setting_config.auto_power_open_time)) {
                    // 不同需要更新
                    memcpy(save_open_time, glob_setting_config.auto_power_open_time,
                           sizeof(save_open_time));
                    sscanf(save_open_time, "%d:%d:%d", &open_hours,
                           &open_minutes, &open_seconds);
                }
                if (timeinfo->hour == open_hours &&
                    timeinfo->min == open_minutes &&
                    timeinfo->sec == open_seconds) {
                    // setup 开机
                    if (!open_is_call) {
                        callback(1);
                        open_is_call = true;
                    }
                    return;
                } else {
                    open_is_call = false;
                }
            }

            // 2. 判断关机条件
            if (strlen(glob_setting_config.auto_power_close_time)) {
                static char save_close_time[9] = {0};
                static int close_hours, close_minutes, close_seconds;
                static bool close_is_call = false;
                if (strcmp(save_close_time,
                           glob_setting_config.auto_power_close_time)) {
                    // 不同需要更新
                    memcpy(save_close_time, glob_setting_config.auto_power_close_time,
                           sizeof(save_close_time));
                    sscanf(save_close_time, "%d:%d:%d", &close_hours,
                           &close_minutes, &close_seconds);
                }
                if (timeinfo->hour == close_hours &&
                    timeinfo->min == close_minutes &&
                    timeinfo->sec == close_seconds) {
                    // stop关机
                    if (!close_is_call) {
                        callback(0);
                        close_is_call = true;
                    }
                    return;
                } else {
                    close_is_call = false;
                }
            }
        }
    }
}

void logic_handler_alarm_clock(rx8025_timeinfo *timeinfo, AlarmClockCallback callback) {
    if (!glob_setting_config.alarm_clock) {
        return;
    }
    // 判断生效条件 今日是否生效
    if (!is_number_in_array(glob_setting_config.alarm_clock_enable_days, 7,
                            timeinfo->week)) {
        return;
    }
    if (strlen(glob_setting_config.alarm_clock_time) == 0) {
        return;
    }
    static char save_alarm_time[9];
    static int alarm_hours, alarm_minutes, alarm_seconds;
    static bool alarm_is_call = false;
    if (strcmp(save_alarm_time, glob_setting_config.alarm_clock_time)) {
        // 需要更新
        memcpy(save_alarm_time, glob_setting_config.alarm_clock_time,
               sizeof(save_alarm_time));
        sscanf(save_alarm_time, "%d:%d:%d", &alarm_hours, &alarm_minutes,
               &alarm_seconds);
    }
    if (timeinfo->hour == alarm_hours && timeinfo->min == alarm_minutes &&
        timeinfo->sec == alarm_seconds) {
        // setup 闹钟时间到了
        if (!alarm_is_call) {
            alarm_is_call = true;
            callback(1);
        }
        return;
    } else {
        alarm_is_call = false;
    }
}

void logic_handler_countdown(rx8025_timeinfo *timeinfo, CountdownCallback callback) {
//    if (!glob_setting_config.countdown) {
//        return;
//    }
//    if (strlen(glob_setting_config.countdown_time) == 0) {
//        return;
//    }
//
//    static u8 s;
//    static u8 count_hours, count_minutes, count_seconds;
//    if (strcmp(countdown_save_time, glob_setting_config.countdown_time)) {
//        memcpy(countdown_save_time, glob_setting_config.countdown_time,
//               sizeof(countdown_save_time));
//        sscanf(countdown_save_time, "%hhd:%hhd:%hhd", &count_hours,
//               &count_minutes, &count_seconds);
//        if (count_seconds == 0 && count_minutes == 0 && count_hours == 0) {
//            logic_handler_countdown_stop();
//            return;
//        }
//        s = timeinfo->sec;
//        callback(1, count_hours, count_minutes, count_seconds);
//    } else {
//        u8 is_change = 0;
//        // 非首次
//        if (count_seconds == 0 && count_minutes == 0 && count_hours == 0) {
//            // 结束
//            logic_handler_countdown_stop();
//            callback(0, count_hours, count_minutes, count_seconds);
//            return;
//        } else {
//            // check sec
//            if (s != timeinfo->sec) {
//                count_seconds--;
//                is_change = 1;
//            }
//            if (count_seconds == 255) {
//                if (count_hours != 0 || count_minutes != 0) {
//                    count_seconds = 59;
//                    count_minutes--;
//                    if (count_minutes == 255) {
//                        if (count_hours != 0) {
//                            count_minutes = 59;
//                            count_hours--;
//                            if (count_hours == 255) {
//                                count_hours = 0;
//                            }
//                        } else {
//                            count_minutes = 0;
//                        }
//                    }
//                } else {
//                    count_seconds = 0;
//                }
//            }
//        }
//        if (is_change) {
//            s = timeinfo->sec;
//            callback(1, count_hours, count_minutes, count_seconds);
//        }
//    }
}

void logic_handler_countdown_stop() {
//    if (!glob_setting_config.countdown) {
//        return;
//    }
//    memset(countdown_save_time, 0, sizeof(countdown_save_time));
//    memset(glob_setting_config.countdown_time, 0, sizeof(glob_setting_config.countdown_time));
//    glob_setting_config.countdown = 0;
//    store_save_setting();
}