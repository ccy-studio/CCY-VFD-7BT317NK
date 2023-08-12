/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-04 14:33:25
 * @LastEditTime: 2023-08-11 22:31:57
 */

// blog-address: saisaiwa.com

#ifndef __PT6315__
#define __PT6315__

#define PT_PLATFORM ESPMCU

#if PT_PLATFORM == ESPMCU
#include <Arduino.h>
#endif

#define CLK_PIN_GROUP 0
#define CLK_PIN 14
#define DIN_PIN_GROUP 0
#define DIN_PIN 12
#define STB_PIN_GROUP 0
#define STB_PIN 16

#if PT_PLATFORM == ESPMCU

#define CLK_1 digitalWrite(CLK_PIN, HIGH)
#define CLK_0 digitalWrite(CLK_PIN, LOW)
#define DIN_1 digitalWrite(DIN_PIN, HIGH)
#define DIN_0 digitalWrite(DIN_PIN, LOW)
#define STB_1 digitalWrite(STB_PIN, HIGH)
#define STB_0 digitalWrite(STB_PIN, LOW)

#define delay_us(us) delayMicroseconds(us)

#endif

#if PT_PLATFORM == STM32
// todo 自己配置STM32的宏定义
#endif

/**
 * 初始化GPIO
 */
void ptInitGPIO(void);

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
void ptSetDisplayLight(uint8_t onOff, uint8_t brightnessVal);
void setModeWirteDisplayMode(uint8_t addressMode = 0);
void setDisplayMode(uint8_t digit);
void sendDigAndData(uint8_t dig, const uint8_t* data, size_t len);
#endif