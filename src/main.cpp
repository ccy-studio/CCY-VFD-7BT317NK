/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-28 21:57:30
 * @LastEditTime: 2023-07-31 21:38:29
 */
// 是否开启AHT20温湿度传感器
#define I2C_AHT20 0

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <gui.h>
#if I2C_AHT20
#include <Adafruit_AHTX0.h>
#include <Wire.h>
Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;
#endif

#define LED_PIN 15
#define KEY1 2
#define KEY2_I2C_SDA 4
#define KEY3_I2C_SCL 5

#define NTP1 "ntp1.aliyun.com"
#define NTP2 "ntp2.aliyun.com"
#define NTP3 "ntp3.aliyun.com"

void read_i2c_aht20();
void set_key_listener();
IRAM_ATTR void handle_key_interrupt();
void getTimeInfo();
void configModeCallback(WiFiManager* myWiFiManager);

u32 key_filter_sec = 0;         // 按键防抖
volatile u32 k1_last_time = 0;  // 按键1的上一次按下触发时间记录

WiFiManager wifiManager;
tm timeinfo;
String time_str = String();
u8 wifi_conn = 0;
u8 mh_state = 0;  // 冒号显示状态

u8 light_level = 1;

#define STYLE_DEFAULT 0
#define STYLE_CUSTOM_1 1
u8 style_page = STYLE_DEFAULT;  // 页面显示样式

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

    vfd_gui_set_text("START.");

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
    vfd_gui_set_icon(ICON_WIFI | ICON_CLOCK);
    vfd_gui_set_text("WAIT..");
    getTimeInfo();
}

void loop() {
    delay(500);
    // Set VFD Text
    getTimeInfo();
    time_str.clear();
    if (style_page == STYLE_DEFAULT) {
        if (wifi_conn) {
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
            vfd_gui_set_text("UNLINK");
        }
    } else if (style_page == STYLE_CUSTOM_1) {
        vfd_gui_set_text("CCY-YC");
        vfd_gui_set_icon(ICON_LEFT_ALL);
    }
}

void read_i2c_aht20() {
#if I2C_AHT20
    detachInterrupt(digitalPinToInterrupt(KEY2_I2C_SDA));
    detachInterrupt(digitalPinToInterrupt(KEY2_I2C_SDA));
    pinMode(KEY2_I2C_SDA, OUTPUT);
    pinMode(KEY3_I2C_SCL, OUTPUT);
    Wire.begin(KEY2_I2C_SDA, KEY3_I2C_SCL);
    if (!aht.begin(&Wire, 0L, 0x38)) {
        Serial.println("Could not find AHT? Check wiring");
        return;
    }
    Serial.println("AHT10 or AHT20 found");
    aht.getEvent(&humidity, &temp);
#endif
}

void set_key_listener() {
    pinMode(KEY2_I2C_SDA, INPUT);
    pinMode(KEY3_I2C_SCL, INPUT);
    // 注册按键中断函数
    attachInterrupt(digitalPinToInterrupt(KEY2_I2C_SDA), handle_key_interrupt,
                    CHANGE);
    attachInterrupt(digitalPinToInterrupt(KEY3_I2C_SCL), handle_key_interrupt,
                    CHANGE);
}

IRAM_ATTR void handle_key_interrupt() {
    u32 filter_sec = (micros() - key_filter_sec) / 1000;
    if (filter_sec < 500) {
        return;
    }
    if (!digitalRead(KEY2_I2C_SDA)) {
        k1_last_time = 0;
        Serial.println("亮度减");
        light_level -= 2;
        if (light_level <= 0) {
            light_level = 1;
        }
        vfd_gui_set_blk_level(light_level);
    }
    if (!digitalRead(KEY3_I2C_SCL)) {
        k1_last_time = 0;
        Serial.println("亮度加");
        light_level += 2;
        if (light_level > 7) {
            light_level = 7;
        }
        vfd_gui_set_blk_level(light_level);
    }
    
    if (!digitalRead(KEY1)) {
        // typec一侧的按键
        Serial.println("KEY1");
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        style_page = !style_page;
        k1_last_time = micros();
    } else if (digitalRead(KEY1)) {
        // 高
        if (digitalRead(KEY2_I2C_SDA) && digitalRead(KEY3_I2C_SCL)) {
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
    vfd_gui_set_text("CONFIG");
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