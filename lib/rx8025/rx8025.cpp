/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:53:37
 * @LastEditTime: 2023-09-24 00:38:54
 */
#include <rx8025.h>

#define RX8025T_ADDR_W 0x64
#define RX8025T_ADDR_R 0x65

#define SDA_1 digitalWrite(I2C_SDA, HIGH)
#define SDA_0 digitalWrite(I2C_SDA, LOW)

#define SCL_1 digitalWrite(I2C_SCL, HIGH)
#define SCL_0 digitalWrite(I2C_SCL, LOW)

rx8025_read_done_fun done_fun;

void i2c_sda_out() {
    pinMode(I2C_SDA, OUTPUT_OPEN_DRAIN);
}

void i2c_sda_in() {
    pinMode(I2C_SDA, INPUT);
}

void i2c_init() {
    detachInterrupt(digitalPinToInterrupt(I2C_SDA));
    detachInterrupt(digitalPinToInterrupt(I2C_SCL));
    i2c_sda_out();
    pinMode(I2C_SCL, OUTPUT_OPEN_DRAIN);
}

void i2c_start() {
    // 开始位（Start）：当SCL=1时，SDA由1向0跳变
    i2c_sda_out();
    SDA_1;
    SCL_1;
    delayMicroseconds(4);
    SDA_0;
    delayMicroseconds(4);
    // 最后拉低SCL数据线准备发送数据进行做准备
    SCL_0;
}

void i2c_stop() {
    // 停止位（Stop）：当SCL=1时，SDA由0向1跳变；
    i2c_sda_out();
    SCL_0;
    SDA_0;
    SCL_1;
    delayMicroseconds(4);
    SDA_1;
    delayMicroseconds(4);
}

void i2c_nack() {
    i2c_sda_out();
    SDA_1;
    SCL_0;
    delayMicroseconds(4);
    SCL_1;
    delayMicroseconds(4);
    SCL_0;
}

void i2c_ack() {
    i2c_sda_out();
    SDA_0;
    SCL_0;
    delayMicroseconds(4);
    SCL_1;
    delayMicroseconds(4);
    SCL_0;
}

u8 i2c_check_ack() {
    uint8_t ack = 0, errorRetry = 3;
    SDA_1;  // 释放SDA
    delayMicroseconds(1);
    i2c_sda_in();
    delayMicroseconds(1);
    SCL_1;
    delayMicroseconds(1);
    while (digitalRead(I2C_SDA)) {
        if (errorRetry <= 0) {
            break;
        }
        delayMicroseconds(1);
        errorRetry--;
    }
    if (errorRetry) {
        ack = 1;
    }
    if (!ack) {
        i2c_stop();
    }
    SCL_0;
    return ack;
}

u8 i2c_write(u8 buf) {
    i2c_sda_out();
    SCL_0;
    delayMicroseconds(4);
    for (int i = 0; i < 8; i++) {
        if (buf & 0x80) {
            SDA_1;
        } else {
            SDA_0;
        }
        delayMicroseconds(4);
        SCL_1;
        delayMicroseconds(4);
        SCL_0;
        buf <<= 1;
    }
    return i2c_check_ack();
}

u8 i2c_read(u8 ack) {
    uint8_t receiveData = 0;
    i2c_sda_in();
    for (int i = 0; i < 8; i++) {
        SCL_0;
        delayMicroseconds(4);
        SCL_1;
        receiveData <<= 1;
        if (digitalRead(I2C_SDA)) {
            receiveData |= 0x01;
        }
        delayMicroseconds(1);
    }
    SCL_0;
    ack ? i2c_ack() : i2c_nack();
    return receiveData;
}

void rx8025_read(u8 address, u8* buf, u8 len) {
    i2c_start();
    if (!i2c_write(RX8025T_ADDR_W)) {
        return;
    }
    if (!i2c_write(address)) {
        return;
    }
    i2c_start();
    if (!i2c_write(RX8025T_ADDR_R)) {
        return;
    }
    for (u8 i = 0; i < len; i++) {
        buf[i] = i2c_read((i == len - 1) ? 0 : 1);
    }
    i2c_stop();
}

void rx8025_write(u8 address, u8* buf, u8 len) {
    i2c_start();
    if (!i2c_write(RX8025T_ADDR_W)) {
        return;
    }
    if (!i2c_write(address)) {
        return;
    }
    for (u8 i = 0; i < len; i++) {
        if (!i2c_write(buf[i])) {
            return;
        }
    }
    i2c_stop();
}

void rx8025t_init(rx8025_read_done_fun fun) {
    done_fun = fun;
    i2c_init();
}

void rx8025_reset() {
    i2c_init();
    u8 command[2];
    command[0] = 0x00;
    command[1] = 0x40;
    rx8025_write(0xe0, command, 2);
    done_fun();
}

u8 toBcd(u8 val) {
    return ((val / 10) << 4) | (val % 10);
}

u8 toDec(u8 bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void rx8025_set_time(u8 year,
                     u8 month,
                     u8 day,
                     u8 week,
                     u8 hour,
                     u8 min,
                     u8 sec) {
    i2c_init();
    u8 command[7];
    command[0] = toBcd(sec);
    command[1] = toBcd(min);
    command[2] = toBcd(hour);
    command[3] = (0x00 >> week) | 0x01;
    command[4] = toBcd(day);
    command[5] = toBcd(month);
    command[6] = toBcd(year);
    rx8025_write(0x00, command, 7);
    done_fun();
}

void rx8025_time_get(rx8025_timeinfo* timeinfo) {
    i2c_init();
    u8 buf[7];
    rx8025_read(0x00, buf, 7);
    timeinfo->sec = toDec(buf[0]);
    timeinfo->min = toDec(buf[1]);
    timeinfo->hour = toDec(buf[2]);
    timeinfo->week = buf[3];
    timeinfo->day = toDec(buf[4]);
    timeinfo->month = toDec(buf[5]);
    timeinfo->year = toDec(buf[6]);
    done_fun();
}

String formart_time(rx8025_timeinfo* timeinfo) {
    // 格式HH:mm:ss
    String time = String();
    if (timeinfo->hour < 9) {
        time += "0";
    }
    time += timeinfo->hour;
    if (timeinfo->min < 9) {
        time += "0";
    }
    time += timeinfo->min;
    if (timeinfo->sec < 9) {
        time += "0";
    }
    time += timeinfo->sec;
    return time;
}

/**
 * 格式日期 YYMMdd
 */
String formart_date(rx8025_timeinfo* timeinfo) {
    String date = String();
    date += timeinfo->year;
    if (timeinfo->month < 9) {
        date += "0";
    }
    date += timeinfo->month;
    if (timeinfo->day < 9) {
        date += "0";
    }
    date += timeinfo->day;
    return date;
}