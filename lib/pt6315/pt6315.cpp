/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-04 14:33:32
 * @LastEditTime: 2023-07-29 12:48:49
 */
#include "pt6315.h"

void ptInitGPIO(void) {
#if PT_PLATFORM == ESPMCU
    pinMode(CLK_PIN, OUTPUT);
    pinMode(DIN_PIN, OUTPUT);
    pinMode(STB_PIN, OUTPUT);
#endif
}

void writeData(uint8_t data, int delayState = 1) {
    // CLK 上升沿将会读取串行数据，DIN 为低位开始LSB
    // CLK PWCLK (Clock Pulse Width) ≥ 400 ns
    // DIN保持(Data Hold Time) ≥ 100 ns,tsetup (Data Setup Time) ≥ 100 ns  time
    // > 200ns
    CLK_0;
    for (int i = 0; i < 8; i++) {
        delay_us(10);
        if (data & 0x01) {
            DIN_1;
        } else {
            DIN_0;
        }
        delay_us(10);
        CLK_1;
        delay_us(10);
        CLK_0;
        data >>= 1;
    }
    if (delayState) {
        delay_us(10);
    }
}

/**
 * DATA SETTING COMMANDS 2
 * @param addressMode 地址模式 0自增, 1固定地址模式
 */
void setModeWirteDisplayMode(uint8_t addressMode) {
    uint8_t command = 0x40;
    if (addressMode) {
        command |= 0x4;
    }
    STB_1;
    delay_us(10);
    STB_0;
    delay_us(10);
    writeData(command);
    delay_us(10);
    STB_1;
}

/**
 * COMMANDS 1 显示模式设置命令
 * 0000 ： 4位， 24段
 * 0001 ：5位， 23段
 * 0010 ： 6位数字， 22段
 * 0011 ： 7位， 21段
 * 0100 ： 8位， 20段
 * 0101 ： 9位， 19段
 * 0110 ： 10位， 18段
 * 0111 ： 11位， 17段
 * 1XXX ： 12位， 16段
 */
void setDisplayMode(uint8_t digit) {
    STB_1;
    delay_us(10);
    STB_0;
    delay_us(10);
    writeData(digit);
    delay_us(10);
    STB_1;
}

/**
 * 显示控制命令  COMMANDS 4
 * @param onOff 0显示关闭，1开启显示
 * @param brightnessVal 亮度占空比 0~7调整
 * 000：脉冲宽度= 1/16 0
 * 001：脉冲宽度= 2/16 1
 * 010 ：脉冲宽度= 4/16 0x2
 * 011 ：脉冲宽度= 10/16 3
 * 100：脉冲宽度= 11/16  4
 * 101 ：脉冲宽度= 12/16 0x5
 * 110：脉冲宽度= 13/16 6
 * 111：脉冲宽度= 14/16 0x7
 */
void ptSetDisplayLight(uint8_t onOff, uint8_t brightnessVal) {
    uint8_t command = 0x80 | brightnessVal;
    if (onOff) {
        command |= 0x8;
    }
    STB_1;
    delay_us(10);
    STB_0;
    delay_us(10);
    //0x8f
    writeData(command);
    delay_us(10);
    STB_1;
}

void sendDigAndData(uint8_t dig, const uint8_t* data, size_t len) {
    STB_1;
    delay_us(10);
    STB_0;
    delay_us(10);
    writeData(0xc0 | dig);
    delay_us(10);
    // 写入数据
    for (size_t i = 0; i < len; i++) {
        writeData(data[i],0);
        // delay_us(10);
    }
    delay_us(10);
    STB_1;
}