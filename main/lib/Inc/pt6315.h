/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 10:04:15
 * @LastEditTime: 2023-10-08 11:38:51
 */


#ifndef __PT6315__
#define __PT6315__

#include "constant.h"

#define CLK_PIN 14
#define DIN_PIN 12
#define STB_PIN 16


#define CLK_1 gpio_set_level(CLK_PIN,1)
#define CLK_0 gpio_set_level(CLK_PIN,0)
#define DIN_1 gpio_set_level(DIN_PIN,1)
#define DIN_0 gpio_set_level(DIN_PIN,0)
#define STB_1 gpio_set_level(STB_PIN,1)
#define STB_0 gpio_set_level(STB_PIN,0)


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
void setModeWirteDisplayMode(uint8_t addressMode);
void setDisplayMode(uint8_t digit);
void sendDigAndData(uint8_t dig, const uint8_t* data, size_t len);
#endif