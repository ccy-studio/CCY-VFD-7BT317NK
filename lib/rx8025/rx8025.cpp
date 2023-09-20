/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:53:37
 * @LastEditTime: 2023-09-20 18:05:10
 */
#include <rx8025.h>

rx8025_read_done_fun done_fun;

void rx8025t_init(rx8025_read_done_fun fun) {
    done_fun = fun;

    // 检查是否有断电过?
    Wire.begin();
    Wire.beginTransmission(RX8025_ADDR);
    Wire.write(0xE);  // 寄存器地址，用于读取日期和时间
    Wire.endTransmission(false);
    size_t recv = Wire.requestFrom(RX8025_ADDR, 1, 1);
    if (recv != 1) {
        printf("rx8025_read_all 读取错误\n");
        done_fun();
        return;
    }
    u8 flag = Wire.read();
    if (flag | 0x02) {
        printf("Rx8025数据丢失重置寄存器标记\n");
        // 数据丢失
        rx8025_reset();
        rx8025_set_all(23, 10, 1, 7, 12, 0, 0);
    } else {
        done_fun();
    }
}

void rx8025_reset() {
    Wire.begin();
    // clean flag
    Wire.beginTransmission(RX8025_ADDR);
    u8 command[3];
    command[0] = 0xe0;
    command[1] = 0x00;
    command[2] = 0x40;
    Wire.write(command, 3);
    Wire.endTransmission(true);

    done_fun();
}

u8 toBcd(u8 val) {
    return ((val / 10) << 4) | (val % 10);
}

u8 toDec(u8 bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void rx8025_set_all(u8 year,
                    u8 month,
                    u8 day,
                    u8 week,
                    u8 hour,
                    u8 min,
                    u8 sec) {
    Wire.begin();
    Wire.beginTransmission(RX8025_ADDR);
    u8 command[8];
    command[0] = 0x00;
    command[1] = toBcd(sec);
    command[2] = toBcd(min);
    command[3] = toBcd(hour);
    command[4] = (0x00 >> week) | 0x01;
    command[5] = toBcd(day);
    command[6] = toBcd(month);
    command[7] = toBcd(year);
    Wire.write(command, 8);
    Wire.endTransmission(true);
    done_fun();
}

void rx8025_read_all(rx8025_timeinfo* timeinfo) {
    Wire.begin();
    Wire.beginTransmission(RX8025_ADDR);
    Wire.write(0x00);  // 寄存器地址，用于读取日期和时间
    Wire.endTransmission(false);
    size_t recv = Wire.requestFrom(RX8025_ADDR, 7, 1);
    if (recv != 7) {
        printf("rx8025_read_all 读取错误\n");
        return;
    }
    timeinfo->sec = toDec(Wire.read());
    timeinfo->min = toDec(Wire.read());
    timeinfo->hour = toDec(Wire.read());
    timeinfo->week = Wire.read();
    timeinfo->day = toDec(Wire.read());
    timeinfo->month = toDec(Wire.read());
    timeinfo->year = toDec(Wire.read());
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
String formart_date(rx8025_timeinfo* timeinfo){
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