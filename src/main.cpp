// 是否开启AHT20温湿度传感器
#define I2C_AHT20 1

#include <Arduino.h>
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

void read_i2c_aht20();
void set_key_listener();
IRAM_ATTR void handle_key_interrupt();

u32 key_filter_sec = 0;  // 按键防抖
u32 k1_last_time = 0;    // 按键1的上一次按下触发时间记录
u8 last_key_pin = 0;     // 记录上一次点击的按键PIN码

void setup() {
    Serial.begin(115200);
    pinMode(KEY1, INPUT);
    pinMode(LED_PIN, OUTPUT);
    // 注册按键中断函数
    attachInterrupt(digitalPinToInterrupt(KEY1), handle_key_interrupt, CHANGE);
    set_key_listener();

    // 默认点亮LED
    digitalWrite(LED_PIN, LOW);
    // 初始化VFD
    delay(3000);
    vfd_gui_init();

    vfd_gui_set_text("1234ABC");
}

void loop() {}

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
    if (!digitalRead(KEY1)) {
        // typec一侧的按键
        k1_last_time = micros();
        last_key_pin = KEY1;
    } else if (digitalRead(KEY1)) {
        // 低电平
        u32 sec = (micros() - k1_last_time) / 1000;
        if (k1_last_time != 0 && sec > 2000 && last_key_pin == KEY1) {
            Serial.println("长按操作触发");
        } else {
            k1_last_time = 0;
        }
    }
    if (!digitalRead(KEY2_I2C_SDA)) {
        k1_last_time = 0;
        last_key_pin = KEY2_I2C_SDA;
    }
    if (!digitalRead(KEY3_I2C_SCL)) {
        k1_last_time = 0;
        last_key_pin = KEY3_I2C_SCL;
    }
    key_filter_sec = micros();
}