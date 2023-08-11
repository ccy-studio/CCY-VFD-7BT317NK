/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-12 14:11:16
 * @LastEditTime: 2023-08-11 16:04:29
 */
#ifndef __VFD_GUI_
#define __VFD_GUI_

#include "pt6315.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define ICON_REC 0x00000200
#define ICON_CLOCK 0x00000400
#define ICON_3D 0x00000800
#define ICON_WIFI 0x00001000
#define ICON_LEFT_ALL 0xFFE1FE
#define ICON_NONE 0  // 清空所有ICON显示

// 字库数量
#define VFD_GUI_FONT_LEN 41

// 灯丝PWM引脚
#define PWM_PIN 13

// 字体结构体
typedef struct {
    u8 font[3];
    char text;
} vfd_gui_fonts;

/**
 * 初始化
 */
void vfd_gui_init();

/**
 * 清空VFD屏幕显示,循环刷新如果使用vfd_gui_set_text方法不需要使用它。
 */
void vfd_gui_clear();

/**
 * 在指定位置显示一个char字符,index从1~6
 */
void vfd_gui_set_one_text(size_t index, char oneChar);

/**
 * 显示一串文字，从0位开始。
 * 不足8位补零(清空显示，方便每次不用调用clear防止闪屏出现)
 */
void vfd_gui_set_text(const char* string);

/**
 * 要点亮的ICON图标，宏定义传参
 */
void vfd_gui_set_icon(u32 buf);
/**
 * 背光开关
 */
void vfd_gui_set_bck(u8 onOff);

/**
 * 设置亮度等级 1~7
 */
void vfd_gui_set_blk_level(size_t level);

/**
 * 第一位冒号，参数bool类型
 */
void vfd_gui_set_maohao1(u8 open);

/**
 * 第二位冒号，参数bool类型
 */
void vfd_gui_set_maohao2(u8 open);

/**
 * 循环滚动展示所有文字,可显示任意长字符内容
 * @param string 要展示的内容字符串
 * @param delay_ms 循环展示刷新频率单位 Ms
 * @param loop_count循环播放的次数
 **/
void vfd_gui_set_long_text(const char* string, u32 delay_ms, size_t loop_count);

#endif