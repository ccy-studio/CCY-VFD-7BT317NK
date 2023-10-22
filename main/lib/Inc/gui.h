
#ifndef __VFD_GUI_
#define __VFD_GUI_

#include "pt6315.h"

#define ICON_REC 0x00000200
#define ICON_CLOCK 0x00000400
#define ICON_3D 0x00000800
#define ICON_WIFI 0x00001000

#define ICON_G1_ALL 0xFFE1FE
#define ICON_G1_STYLE_1 0x010000
#define ICON_G1_STYLE_2 0x040000
#define ICON_G1_STYLE_3 0x080000
#define ICON_G1_STYLE_4 0x100000
#define ICON_G1_STYLE_5 0x200000
#define ICON_G1_STYLE_6 0x400000
#define ICON_G1_STYLE_7 0x800000
#define ICON_G1_STYLE_8 0x020000
#define ICON_G1_STYLE_9 0x000100

#define ICON_NONE 0  // 清空所有ICON显示

// VFD位数
#define VFD_DIG_LEN 6

// 灯丝PWM引脚
//#define PWM_PIN 13 //OLD
#define PWM_PIN 15 //New

/**
 * 初始化
 */
void vfd_gui_init();

/**
 * 停止关闭显示、灯丝将停止驱动
 */
void vfd_gui_stop();

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
 * (自动清空覆盖显示，方便每次不用调用clear防止闪屏出现)
 */
u8 vfd_gui_set_text(const char* string, const u8 colon);

/**
 * 要点亮的ICON图标，宏定义传参
 */
void vfd_gui_set_icon(u32 buf);

/**
 * 获取保存的icon图标
 */
u32 vfd_gui_get_save_icon(void);

/**
 * 设置保存的ICON图标
 * @param icon
 */
void vfd_set_save_icon(u32 icon);

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
 * @param loop_count 循环播放的次数
 **/
void vfd_gui_set_long_text(const char* string, u32 delay_ms, size_t loop_count);
/**
 * 如果调用vfd_gui_set_long_text有存在循环播放未结束的调用此方法将会强制结束
 */
void vfd_gui_cancel_long_text();

/**
 * 获取字库值，内部用
 */
u32 gui_get_font(char c);

/**
 * G1动画特效
 */
void vfd_gui_anno_for_g1();
#endif