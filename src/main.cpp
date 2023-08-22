/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-28 21:57:30
 * @LastEditTime: 2023-08-22 11:38:13
 */

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <gui.h>

#define LED_PIN 15
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
void task_led_fun();

u32 key_filter_sec = 0;         // 按键防抖
volatile u32 k1_last_time = 0;  // 按键1的上一次按下触发时间记录

WiFiManager wifiManager;
tm timeinfo;
String time_str = String();
u8 wifi_conn = 0;
u8 mh_state = 0;   // 冒号显示状态
u8 led_state = 1;  // LED呼吸灯特性

u8 light_level = 7;  // 亮度等级

#define STYLE_DEFAULT 0
#define STYLE_CUSTOM_1 1
u8 style_page = STYLE_DEFAULT;  // 页面显示样式

// 定时器初始化
Ticker task_anno;  // 动画执行器
Ticker task_led;   // 动画执行器

void init_tick();

void setup() {
    Serial.begin(115200);
    pinMode(KEY1, INPUT);
    pinMode(LED_PIN, OUTPUT);
    // 注册按键中断函数
    attachInterrupt(digitalPinToInterrupt(KEY1), handle_key_interrupt, CHANGE);
    set_key_listener();

    digitalWrite(LED_PIN, HIGH);
    // 初始化VFD
    delay(500);
    vfd_gui_init();
    vfd_gui_set_blk_level(light_level);
    vfd_gui_set_text("V1.1");

    printf("WIFI SSID:%s\n", wifiManager.getWiFiSSID().c_str());
    printf("WIFI PWD:%s\n", wifiManager.getWiFiPass().c_str());

    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setCountry("CN");
    wifiManager.setBreakAfterConfig(true);
    wifiManager.setTimeout(60);
    // wifiManager.setDebugOutput(false);
    String ssid = "VFD-" + String(ESP.getChipId());
    if (!wifiManager.autoConnect(ssid.c_str(), NULL)) {
        Serial.println("Failed to connect and hit timeout.");
        while (1) {
            delay(500);
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            vfd_gui_set_text("ERROR.");
        }
    }

    // 配网成功，打印连接信息
    Serial.println("Connected to WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifi_conn = 1;
    init_tick();
    vfd_gui_set_long_text("wifi connected", 210, 1);
    vfd_gui_set_long_text(WiFi.localIP().toString().c_str(), 210, 1);
    vfd_gui_set_icon(ICON_WIFI | ICON_CLOCK, 1);
    vfd_gui_set_text("wait.");
    getTimeInfo();
}

void loop() {
    // Set VFD Text
    getTimeInfo();
    time_str.clear();
    if (style_page == STYLE_DEFAULT) {
        if (wifi_conn) {
            delay(500);
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
        } else {
            // 如果联网失败
            vfd_gui_set_long_text("Network disconnection", 100, 2);
        }
    } else if (style_page == STYLE_CUSTOM_1) {
        vfd_gui_set_long_text(">Hello-VFD<", 200, 3);
    }
}

void set_key_listener() {
    pinMode(KEY2, INPUT);
    pinMode(KEY3, INPUT);
    // 注册按键中断函数
    attachInterrupt(digitalPinToInterrupt(KEY2), handle_key_interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(KEY3), handle_key_interrupt, CHANGE);
}

IRAM_ATTR void handle_key_interrupt() {
    u32 filter_sec = (micros() - key_filter_sec) / 1000;
    if (filter_sec < 500) {
        return;
    }
    if (!digitalRead(KEY1) && !digitalRead(KEY2) && digitalRead(KEY3)) {
        led_state = !led_state;
    } else if (!digitalRead(KEY1)) {
        k1_last_time = 0;
        Serial.println("Light-");
        if (light_level == 2) {
            light_level = 1;
        } else if (light_level == 7) {
            light_level = 2;
        }
        vfd_gui_set_blk_level(light_level);
    } else if (!digitalRead(KEY3)) {
        k1_last_time = 0;
        Serial.println("Light+");
        if (light_level == 2) {
            light_level = 7;
        } else if (light_level == 1) {
            light_level = 2;
        }
        vfd_gui_set_blk_level(light_level);
    } else if (!digitalRead(KEY2)) {
        Serial.println("FN");
        style_page = !style_page;
        k1_last_time = micros();
        vfd_gui_cancel_long_text();
    } else if (digitalRead(KEY2)) {
        // 高
        if (digitalRead(KEY1) && digitalRead(KEY3)) {
            u32 sec = (micros() - k1_last_time) / 1000;
            if (k1_last_time != 0 && sec > 2000) {
                Serial.println("长按操作触发");
                // 如果长按到松下有2秒,执行重置WIFI的操作
                wifiManager.erase();
                ESP.restart();
            } else {
                k1_last_time = 0;
            }
        }
    }
    key_filter_sec = micros();
}

void configModeCallback(WiFiManager* myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
    digitalWrite(LED_PIN, LOW);
    vfd_gui_clear();
    vfd_gui_set_text("ap-run");
}

void getTimeInfo() {
    if (!getLocalTime(&timeinfo)) {
        if (WiFi.isConnected()) {
            configTime(8 * 3600, 0, NTP1, NTP2, NTP3);
        } else {
            wifi_conn = 0;
            vfd_gui_set_icon(ICON_NONE);
            vfd_gui_set_maohao1(0);
            vfd_gui_set_maohao2(0);
        }
    }
}

/**
 * 执行器-> G1动画执行器
 */
void task_anno_fun() {
    vfd_gui_anno_for_g1(0);
}

/**
 * 执行器-> LED呼吸灯
 */
void task_led_fun() {
    if (!led_state) {
        return;
    }
    static u8 direction = 1;
    static u8 _led_pwm = 0;
    analogWrite(LED_PIN, _led_pwm);
    if (direction) {
        _led_pwm++;
    } else {
        _led_pwm--;
    }
    if (_led_pwm >= 255) {
        direction = 0;
    } else if (_led_pwm <= 0) {
        direction = 1;
    }
}

void init_tick() {
    // G1动画执行 帧率160ms
    task_anno.attach_ms(160, task_anno_fun);
    task_led.attach_ms(50, task_led_fun);
}