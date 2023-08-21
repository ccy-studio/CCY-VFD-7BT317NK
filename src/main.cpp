/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-28 21:57:30
 * @LastEditTime: 2023-08-21 21:40:07
 */

#include <Arduino.h>
#include <Ticker.h>
#include <buzzer.h>
#include <constant.h>
#include <gui.h>
#include <rgb.h>
#include <service.h>
#include <web_server.h>

#define KEY1 2
#define KEY2 4
#define KEY3 5

#define NTP1 "ntp1.aliyun.com"
#define NTP2 "ntp2.aliyun.com"
#define NTP3 "ntp3.aliyun.com"

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

u8 power = 1;  // 电源

u32 key_filter_sec = 0;  // 按键防抖
u32 k1_last_time = 0;    // 按键1的上一次按下触发时间记录
u8 key_last_pin = 0;

tm timeinfo;  // 时间对象
String time_str = String();
u8 mh_state = 0;     // 冒号显示状态
u8 light_level = 7;  // 亮度等级

#define STYLE_DEFAULT 0
#define STYLE_CUSTOM_1 1
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

    // RBG初始化
    rgb_setup();

    web_setup(configModeCallback, configModeTimeoutError);

    vfd_gui_set_long_text("wifi connected", 210, 1);
    vfd_gui_set_long_text(WiFi.localIP().toString().c_str(), 210, 1);
    vfd_gui_set_icon(ICON_CLOCK, 1);
    vfd_gui_set_text("wait.");
    getTimeInfo();
    buzzer_play_di();
}

void loop() {
    getTimeInfo();
    if (power) {
        web_loop();
        set_tick();
        vfd_synchronous();
    }
    // 开关机处理
    logic_handler_auto_power(&timeinfo, power_handle);
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
    if (!getLocalTime(&timeinfo)) {
        if (WiFi.isConnected()) {
            configTime(8 * 3600, 0, NTP1, NTP2, NTP3);
        }
    }
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
        style_page = !style_page;
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
    time_str.clear();
    if (style_page == STYLE_DEFAULT) {
        // 拼接时间字符串格式： HH:mm:ss
        time_str += (timeinfo.tm_hour < 10 ? "0" : "");
        time_str += timeinfo.tm_hour;
        time_str += (timeinfo.tm_min < 10 ? "0" : "");
        time_str += timeinfo.tm_min;
        time_str += (timeinfo.tm_sec < 10 ? "0" : "");
        time_str += timeinfo.tm_sec;
        vfd_gui_set_text(time_str.c_str());
        vfd_gui_set_maohao1(mh_state);
        vfd_gui_set_maohao2(mh_state);
        mh_state = !mh_state;
    }
    if (WiFi.isConnected()) {
        vfd_gui_set_icon(ICON_WIFI | vfd_gui_get_save_icon(), 0);
    }
}

void set_tick() {
    if (!task_time_refresh.active()) {
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
            vfd_gui_set_icon(vfd_gui_get_save_icon(), 0);
        }
    }

    // rgb设置
    if (setting_obj.rgb_open) {
        if (!task_led.active()) {
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
    }
}

/**
 * 处理开关机
 */
void power_handle(u8 state) {
    power = state;
    if (!state) {
        task_time_refresh.detach();
        task_anno.detach();
        task_led.detach();
        rgb_clear();
        vfd_gui_cancel_long_text();
        vfd_gui_stop();
        web_stop();
    } else {
        vfd_gui_set_icon(vfd_gui_get_save_icon(), 1);
        style_page = STYLE_DEFAULT;
        vfd_gui_init();
    }
}
