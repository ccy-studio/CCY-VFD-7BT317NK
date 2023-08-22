/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系 [yustart@foxmail.com] 进行授权。
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
 * @Date: 2023-07-04 14:33:32
 * @LastEditTime: 2023-08-11 23:22:20
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
    // 0x8f
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
        writeData(data[i], 0);
    }
    delay_us(10);
    STB_1;
}
