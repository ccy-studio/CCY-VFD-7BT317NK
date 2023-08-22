/*
 * @Description: 
 * @Author: chenzedeng
 * @Date: 2023-08-22 20:54:11
 * @LastEditTime: 2023-08-22 22:20:36
 */
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
 * @Author: chenzedeng
 * @Date: 2023-07-28 21:57:30
 * @LastEditTime: 2023-08-22 16:28:54
 */

#include <Arduino.h>
#include <NTPClient.h>
#include <Ticker.h>
#include <WiFiUdp.h>
#include <buzzer.h>
#include <coredecls.h>
#include <gui.h>
#include <rgb.h>
#include <service.h>
#include <web_server.h>

void set_key_listener();
IRAM_ATTR void handle_key_interrupt();
void getTimeInfo();
void configModeCallback(WiFiManager* myWiFiManager);
void configModeTimeoutError();
void set_tick();
void task_anno_fun();
void task_led_fun();
void task_time_refresh_fun();
void vfd_synchronous();
void power_handle(u8 state);
void alarm_handle(u8 state);
void countdown_handle(u8 state, u8 hour, u8 min, u8 sec);

u8 power = 1;       // 电源
u8 countdounw = 0;  // 是否开启计数器模式

u32 key_filter_sec = 0;  // 按键防抖
u32 k1_last_time = 0;    // 按键1的上一次按下触发时间记录
u8 key_last_pin = 0;     // 记录上一次按下的按键PIN

tm timeinfo;  // 时间对象
time_t now;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP3, 8 * 3600, 21600000);  // 6小时更新一次
String time_str = String();
u8 mh_state = 0;     // 冒号显示状态
u8 light_level = 7;  // 亮度等级

const u8 style_max = 3;         // 页面展示样式支持总数
u8 style_page = STYLE_DEFAULT;  // 页面显示样式

// 定时器初始化
Ticker task_anno;          // 动画执行器
Ticker task_led;           // LED执行器
Ticker task_time_refresh;  // 时间刷新

void setup() {
    Serial.begin(115200);

    set_key_listener();

    // 蜂鸣器初始化
    buzzer_setup(handle_key_interrupt);

    // 初始化VFD
    delay(500);
    vfd_gui_init();
    vfd_gui_set_blk_level(light_level);
    vfd_gui_set_text("vfd-03");
    delay(500);
    vfd_gui_set_text("start.");

    // 初始化StoreFS
    store_init();
    // 读数据
    store_get_setting(&setting_obj);

    web_setup(configModeCallback, configModeTimeoutError);

    vfd_gui_set_long_text("wifi connected", 210, 1);
    vfd_gui_set_long_text(WiFi.localIP().toString().c_str(), 210, 1);
    vfd_gui_set_text("load.");

    timeClient.begin();

    getTimeInfo();
    buzzer_play_di();
}

void loop() {
    getTimeInfo();
    if (power) {
        web_loop();
        set_tick();
        vfd_synchronous();
        logic_handler_countdown(&timeinfo, countdown_handle);
    }
    // 开关机处理
    logic_handler_auto_power(&timeinfo, power_handle);
    logic_handler_alarm_clock(&timeinfo, alarm_handle);
}

void set_key_listener() {
    pinMode(KEY1, INPUT);
    pinMode(KEY2, INPUT);
    pinMode(KEY3, INPUT);
    // 注册按键中断函数
    attachInterrupt(digitalPinToInterrupt(KEY1), handle_key_interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(KEY2), handle_key_interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(KEY3), handle_key_interrupt, CHANGE);
}

void configModeTimeoutError() {
    delay(500);
    vfd_gui_set_long_text("connect timeout Please power up again!", 200, 1);
}

void configModeCallback(WiFiManager* myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
    vfd_gui_clear();
    vfd_gui_set_text("ap-run");
}

void getTimeInfo() {
    timeClient.update();
    now = timeClient.getEpochTime();
    localtime_r(&now, &timeinfo);
}

//////////////////////////////////////////////////////////////////////////////////
//// 外部按键中断处理
//////////////////////////////////////////////////////////////////////////////////

IRAM_ATTR void handle_key_interrupt() {
    u32 filter_sec = (micros() - key_filter_sec) / 1000;
    if (filter_sec < 500) {
        return;
    }
    if (!power) {
        // 如果是休眠关机下，触发按键直接开机
        power_handle(1);
        key_filter_sec = micros();
        return;
    }
    if (countdounw) {
        // 如果是计时器模式，点击按键则取消计时
        countdounw = 0;
        logic_handler_countdown_stop();
        key_filter_sec = micros();
        return;
    }
    if (digitalRead(KEY3) && !digitalRead(KEY1) && !digitalRead(KEY2)) {
        // WIFI设置打开或者关闭
        if (WiFi.isConnected()) {
            // 如打开就关闭
            web_stop();
        } else {
            // 如果关闭就打开
            // 打开比较耗时且还要检测是否配网，一套流程下来其实就是setup内初始化了.倒不如重启ESP
            ESP.restart();
        }
    } else if (!digitalRead(KEY1)) {
        key_last_pin = KEY1;
        k1_last_time = 0;
        Serial.println("Light-");
        if (light_level == 2) {
            light_level = 1;
        } else if (light_level == 7) {
            light_level = 2;
        }
        vfd_gui_set_blk_level(light_level);
    } else if (!digitalRead(KEY2)) {
        key_last_pin = KEY2;
        k1_last_time = 0;
        Serial.println("Light+");
        if (light_level == 2) {
            light_level = 7;
        } else if (light_level == 1) {
            light_level = 2;
        }
        vfd_gui_set_blk_level(light_level);
    } else if (!digitalRead(KEY3)) {
        key_last_pin = KEY3;
        Serial.println("FN");
        style_page = (style_page + 1) % style_max;
        k1_last_time = micros();
        vfd_gui_cancel_long_text();
    } else if (digitalRead(KEY3)) {
        // 高
        if (digitalRead(KEY1) && digitalRead(KEY2) && key_last_pin == KEY1) {
            u32 sec = (micros() - k1_last_time) / 1000;
            if (k1_last_time != 0 && sec > 2000) {
                Serial.println("长按操作触发");
                // 如果长按到松下有2秒,执行重置WIFI的操作
                wifi_erase();
                ESP.restart();
            } else {
                k1_last_time = 0;
            }
        }
    }
    key_filter_sec = micros();
}

//////////////////////////////////////////////////////////////////////////////////
//// Task 异步任务
//////////////////////////////////////////////////////////////////////////////////

/**
 * 执行器-> G1动画执行器
 */
void task_anno_fun() {
    vfd_gui_anno_for_g1(0);
}

void task_led_fun() {
    rgb_set_style(setting_obj.rgb_style);
    rgb_set_brightness(setting_obj.rgb_brightness);
    rbg_frame_update();
}

void task_time_refresh_fun() {
    if (style_page == STYLE_DEFAULT) {
        String timeStr = timeClient.getFormattedTime();
        timeStr.replace(":", "");
        vfd_gui_set_text(timeStr.c_str());
        vfd_gui_set_maohao1(mh_state);
        vfd_gui_set_maohao2(mh_state);
        mh_state = !mh_state;
    }
    vfd_gui_set_icon(ICON_CLOCK, 1);
    if (WiFi.isConnected()) {
        vfd_gui_set_icon(ICON_WIFI | vfd_gui_get_save_icon(), 0);
    }
}

void set_tick() {
    if (!countdounw && !task_time_refresh.active()) {
        task_time_refresh.attach_ms(VFD_TIME_FRAME, task_time_refresh_fun);
    }

    // G1动画设置
    if (setting_obj.anno_open) {
        if (!task_anno.active()) {
            task_anno.attach_ms(G1_ANNO_FRAME, task_anno_fun);
        }
    } else {
        if (task_anno.active()) {
            task_anno.detach();
        }
    }

    // rgb设置
    if (setting_obj.rgb_open) {
        if (!task_led.active()) {
            rgb_setup();
            task_led.attach_ms(RGB_ANNO_FRAME, task_led_fun);
        }
    } else {
        if (task_led.active()) {
            task_led.detach();
            rgb_clear();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
//// 同步阻塞任务
//////////////////////////////////////////////////////////////////////////////////

void vfd_synchronous() {
    if (style_page == STYLE_CUSTOM_1) {
        static char long_text[50];
        if (strlen(setting_obj.custom_long_text) != 0 &&
            strcmp(long_text, setting_obj.custom_long_text)) {
            delay(50);
            memset(long_text, 0, sizeof(long_text));
            memcpy(long_text, setting_obj.custom_long_text,
                   sizeof(setting_obj.custom_long_text));
        }
        vfd_gui_set_long_text(long_text, setting_obj.custom_long_text_frame, 2);
    } else if (style_page == STYLE_CUSTOM_2) {
        if (WiFi.isConnected()) {
            vfd_gui_set_long_text(WiFi.localIP().toString().c_str(), 210, 1);
        } else {
            vfd_gui_set_long_text("WiFi not connected", 210, 1);
        }
    }
}

/**
 * 处理开关机
 */
void power_handle(u8 state) {
    if (power == state) {
        return;
    }
    if (!state) {
        web_stop();
        task_time_refresh.detach();
        task_anno.detach();
        task_led.detach();
        rgb_clear();
        vfd_gui_cancel_long_text();
        vfd_gui_stop();
    } else {
        style_page = STYLE_DEFAULT;
        vfd_gui_init();
        rgb_setup();
    }
    power = state;
}

/*
 闹钟处理
*/
void alarm_handle(u8 state) {
    // 处理状态
    static u8 handle_state = 1;
    if (handle_state) {
        // 已处理
        handle_state = 0;
        task_time_refresh.detach();
        vfd_gui_set_text("alarm.");
        for (size_t i = 0; i < 10; i++) {
            delay(350);
            buzzer_play_di(100);
        }
        // 重置到待处理
        handle_state = 0;
    }
}

/**
 * 计数器处理
 */
void countdown_handle(u8 state, u8 hour, u8 min, u8 sec) {
    countdounw = state;
    if (countdounw) {
        task_time_refresh.detach();
        time_str.clear();
        // 拼接时间字符串格式： HH:mm:ss
        time_str += (hour < 10 ? "0" : "");
        time_str += hour;
        time_str += (min < 10 ? "0" : "");
        time_str += min;
        time_str += (sec < 10 ? "0" : "");
        time_str += sec;
        vfd_gui_set_text(time_str.c_str());
        vfd_gui_set_icon(ICON_REC, 1);
        if (WiFi.isConnected()) {
            vfd_gui_set_icon(ICON_WIFI | vfd_gui_get_save_icon(), 0);
        }
    } else {
        for (size_t i = 0; i < 3; i++) {
            delay(200);
            buzzer_play_di(100);
        }
    }
}
