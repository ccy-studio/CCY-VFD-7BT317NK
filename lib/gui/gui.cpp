/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-12 14:14:04
 * @LastEditTime: 2023-08-11 17:25:32
 */
#include "gui.h"

extern vfd_gui_fonts fonts[VFD_GUI_FONT_LEN];
extern vfd_gui_fonts icons[6];

u8 lightOff = 1;    // 背光开关
u8 lightLevel = 2;  // 亮度级别

u8 mh1, mh2;  // 冒号1、2

void vfd_gui_init() {
    // 初始化GPIO
    ptInitGPIO();
    pinMode(PWM_PIN, OUTPUT);
    // 设置PWM的频率单位hz
    analogWriteFreq(20000);  // V2版本用的频率
    analogWrite(PWM_PIN, 25);
    // VFD Setting
    setDisplayMode(3);                        // command1
    ptSetDisplayLight(lightOff, lightLevel);  // command4
    vfd_gui_clear();
}

void vfd_gui_clear() {
    setModeWirteDisplayMode(0);  // command2
    u8 clearBuf[24];
    memset(clearBuf, 0x00, sizeof(clearBuf));
    setModeWirteDisplayMode(0);       // command2
    sendDigAndData(0, clearBuf, 24);  // command3
}

void vfd_gui_set_one_text(size_t index, char oneChar) {
    u8 buf[3];
    memset(buf, 0, sizeof(buf));
    for (size_t i = 0; i < VFD_GUI_FONT_LEN; i++) {
        if (fonts[i].text == oneChar ||
            (oneChar >= 97 && fonts[i].text == (oneChar - 32))) {
            buf[0] = fonts[i].font[0];
            buf[1] = fonts[i].font[1];
            buf[2] = fonts[i].font[2];
            break;
        }
    }
    setModeWirteDisplayMode(0);         // command2
    sendDigAndData(index * 3, buf, 3);  // command3
}

void vfd_gui_set_icon(u32 buf) {
    uint8_t arr[3];
    arr[0] = (buf >> 16) & 0xFF;
    arr[1] = (buf >> 8) & 0xFF;
    arr[2] = buf & 0xFF;
    setModeWirteDisplayMode(0);  // command2
    sendDigAndData(0, arr, 3);   // command3
}

void vfd_gui_set_text(const char* string) {
    size_t str_len = strlen(string);
    if (str_len > 6) {
        printf("长度不可以大于6位可显示的区域!");
        return;
    }
    u8 data[24];
    memset(data, 0, sizeof(data));
    size_t index = 0;
    for (size_t i = 0; i < str_len; i++) {
        for (size_t j = 0; j < VFD_GUI_FONT_LEN; j++) {
            if (string[i] == fonts[j].text ||
                (string[i] >= 97 && fonts[j].text == (string[i] - 32))) {
                data[index++] = fonts[j].font[0];
                data[index++] = fonts[j].font[1];
                data[index++] = fonts[j].font[2];
                break;
            }
        }
    }
    if (str_len >= 2) {
        mh1 = data[5];
    } else {
        mh1 = 0;
    }
    if (str_len >= 4) {
        mh2 = data[11];
    } else {
        mh2 = 0;
    }
    setModeWirteDisplayMode(0);   // command2
    sendDigAndData(3, data, 24);  // command3
}

void vfd_gui_set_bck(u8 onOff) {
    lightOff = onOff;
    ptSetDisplayLight(lightOff, lightLevel);  // command4
}

/**
 * 设置亮度等级 1~7
 */
void vfd_gui_set_blk_level(size_t level) {
    lightLevel = level;
    ptSetDisplayLight(lightOff, lightLevel);  // command4
}

static void vfd_set_maohao(u8 address, u8 buf) {
    setModeWirteDisplayMode(1);        // command2
    sendDigAndData(address, &buf, 1);  // command3
}

void vfd_gui_set_maohao1(u8 open) {
    u8 command = open ? mh1 | 0x10 : mh1;
    vfd_set_maohao(0x08, command);
}
void vfd_gui_set_maohao2(u8 open) {
    u8 command = open ? mh2 | 0x10 : mh2;
    vfd_set_maohao(0x0E, command);
}

/**
 * 循环滚动展示所有文字,可显示任意长字符内容
 * @param string 要展示的内容字符串
 * @param delay_ms 循环展示刷新频率单位 Ms
 * @param loop_count循环播放的次数
 **/
void vfd_gui_set_long_text(const char* string,
                           u32 delay_ms,
                           size_t loop_count) {
    loop_count = loop_count <= 0 ? 1 : loop_count;
    // 屏幕位数
    static const size_t display_dig_count = 6;
    // 长度共计6位 + \0 所以单个数组长度为7位
    static const size_t arr_sub_len = display_dig_count + 1;
    // 刷新的页码数
    size_t pageSize = 1;
    size_t str_len = strlen(string);
    size_t arr_len;
    // 如果大于6满整屏不够显示的
    if (str_len > display_dig_count) {
        pageSize += (str_len - display_dig_count);
        // pageSize += 5;是渐渐消失需要的页码数量 总数6-1
        pageSize += display_dig_count - 1;
    } else {
        // 足够显示或者不满屏幕 str_len - 1;是渐渐消失需要的页码数量
        pageSize += str_len - 1;
    }
    // 正向渐出+动画渐出清屏+反向渐入+反向清屏
    pageSize += loop_count > 1 ? str_len + display_dig_count - 1 : 0;
    arr_len = pageSize * arr_sub_len;
    // 分配内存
    char* buf = (char*)malloc(arr_len * sizeof(char));
    char* zreo_point = buf;
    char* second_point;
    if (buf == NULL) {
        printf("分配内存失败");
        return;
    }
    // 初始化数组
    memset(buf, ' ', arr_len * sizeof(char));

    // 正向渐出初始化数据
    for (size_t i = 0; i < str_len; i++) {
        strncpy(buf, string + i, display_dig_count);
        buf += display_dig_count;
        *buf = '\0';
        buf++;
    }

    // 如果循环次数大于1次就初始化动画效果所需要的数据
    if (loop_count > 1) {
        second_point = buf;
        // 初始化反转数据数组
        for (size_t i = 0; i < str_len; i++) {
            if (i != 0) {
                memcpy(buf, (buf - arr_sub_len + 1),
                       (arr_sub_len - 1) * sizeof(char));
            }
            // 6-1 = 5
            buf += display_dig_count - 1;
            *buf = string[i];
            buf++;
            *buf = '\0';
            buf++;
        }
        // 完全渐出
        for (size_t i = 0; i < display_dig_count - 1; i++) {
            memcpy(buf, (buf - arr_sub_len + 1),
                   (arr_sub_len - 1) * sizeof(char));
            buf += display_dig_count - 1;
            *buf = ' ';
            buf++;
            *buf = '\0';
            buf++;
        }
    }

    // 输出
    // 将buf指针位置重新赋值到初始值
    buf = zreo_point;
    // 循环两次或者1次
    while (buf && *buf != '\0') {
        vfd_gui_set_text(buf);
        delay(delay_ms);
        buf += arr_sub_len;
    }
    // loop_count大于2的时候开始根据loop_count的数量-2循环到截止.
    for (size_t i = 0; i < loop_count - 2; i++) {
        buf = second_point;
        while (buf && *buf != '\0') {
            vfd_gui_set_text(buf);
            delay(delay_ms);
            buf += arr_sub_len;
        }
    }
    // 释放内存空间
    free(zreo_point);
}