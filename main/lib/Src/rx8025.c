/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:53:37
 * @LastEditTime: 2023-10-10 15:12:47
 */
#include <rx8025.h>

#define RX8025T_ADDR_W 0x64
#define RX8025T_ADDR_R 0x65

#define SDA_1 gpio_set_level(I2C_SDA, 1)
#define SDA_0 gpio_set_level(I2C_SDA, 0)

#define SCL_1 gpio_set_level(I2C_SCL, 1)
#define SCL_0 gpio_set_level(I2C_SCL, 0)

static fun_key_action key_func;

void i2c_sda_out() {
    gpio_set_direction(I2C_SDA, GPIO_MODE_OUTPUT_OD);
    gpio_pullup_dis(I2C_SDA);
    gpio_pulldown_dis(I2C_SDA);
}

void i2c_sda_in() {
    gpio_set_direction(I2C_SDA, GPIO_MODE_INPUT);
    gpio_pullup_dis(I2C_SDA);
    gpio_pulldown_dis(I2C_SDA);
}

void i2c_init() {
    i2c_sda_out();
    gpio_set_direction(I2C_SCL, GPIO_MODE_OUTPUT_OD);
    gpio_pullup_dis(I2C_SCL);
    gpio_pulldown_dis(I2C_SCL);
}

void i2c_start() {
    // 开始位（Start）：当SCL=1时，SDA由1向0跳变
    i2c_sda_out();
    SDA_1;
    SCL_1;
    delay_us(4);
    SDA_0;
    delay_us(4);
    // 最后拉低SCL数据线准备发送数据进行做准备
    SCL_0;
}

void i2c_stop() {
    // 停止位（Stop）：当SCL=1时，SDA由0向1跳变；
    i2c_sda_out();
    SCL_0;
    SDA_0;
    SCL_1;
    delay_us(4);
    SDA_1;
    delay_us(4);
}

void i2c_nack() {
    i2c_sda_out();
    SDA_1;
    SCL_0;
    delay_us(4);
    SCL_1;
    delay_us(4);
    SCL_0;
}

void i2c_ack() {
    i2c_sda_out();
    SDA_0;
    SCL_0;
    delay_us(4);
    SCL_1;
    delay_us(4);
    SCL_0;
}

u8 i2c_check_ack() {
    uint8_t ack = 0, errorRetry = 3;
    SDA_1;  // 释放SDA
    delay_us(1);
    i2c_sda_in();
    delay_us(1);
    SCL_1;
    delay_us(1);
    while (gpio_get_level(I2C_SDA)) {
        if (errorRetry <= 0) {
            break;
        }
        delay_us(1);
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
    delay_us(4);
    for (int i = 0; i < 8; i++) {
        if (buf & 0x80) {
            SDA_1;
        } else {
            SDA_0;
        }
        delay_us(4);
        SCL_1;
        delay_us(4);
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
        delay_us(4);
        SCL_1;
        receiveData <<= 1;
        if (gpio_get_level(I2C_SDA)) {
            receiveData |= 0x01;
        }
        delay_us(1);
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

void rx8025t_init(fun_key_action fun) {
    key_func = fun;
    i2c_init();
}

void rx8025_reset() {
    key_func(1);
    i2c_init();
    u8 command[2];
    command[0] = 0x00;
    command[1] = 0x40;
    rx8025_write(0xe0, command, 2);
    key_func(0);
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
    key_func(1);
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
    key_func(0);
}

void rx8025_time_get(rx8025_timeinfo* timeinfo) {
    key_func(1);
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
    key_func(0);
}

void formart_time(rx8025_timeinfo* timeinfo, char* buf, size_t buf_size) {
    // 格式HH:mm:ss
    memset(buf, 0, buf_size);

    if (timeinfo->hour < 10) {
        snprintf(buf, buf_size, "0%d", timeinfo->hour);
    } else {
        snprintf(buf, buf_size, "%d", timeinfo->hour);
    }

    if (timeinfo->min < 10) {
        snprintf(buf + strlen(buf), buf_size, "0%d", timeinfo->min);
    } else {
        snprintf(buf + strlen(buf), buf_size, "%d", timeinfo->min);
    }

    if (timeinfo->sec < 10) {
        snprintf(buf + strlen(buf), buf_size, "0%d", timeinfo->sec);
    } else {
        snprintf(buf + strlen(buf), buf_size, "%d", timeinfo->sec);
    }
}

/**
 * 格式日期 YYMMdd
 */
void formart_date(rx8025_timeinfo* timeinfo, char* buf, size_t buf_size) {
    memset(buf, 0, sizeof(char) * buf_size);

    snprintf(buf + strlen(buf), buf_size, "%d", timeinfo->year);

    if (timeinfo->month < 10) {
        snprintf(buf + strlen(buf), buf_size, "0%d", timeinfo->month);
    } else {
        snprintf(buf + strlen(buf), buf_size, "%d", timeinfo->month);
    }
    if (timeinfo->day < 10) {
        snprintf(buf + strlen(buf), buf_size, "0%d", timeinfo->day);
    } else {
        snprintf(buf + strlen(buf), buf_size, "%d", timeinfo->day);
    }
}