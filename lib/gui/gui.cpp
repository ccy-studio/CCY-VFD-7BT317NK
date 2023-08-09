/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-12 14:14:04
 * @LastEditTime: 2023-08-09 23:46:29
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
    // analogWriteFreq(8000); //V1版本
    // analogWrite(PWM_PIN, 128);
    
    analogWriteFreq(20000); //V2版本用的频率
    analogWrite(PWM_PIN, 25);
    //VFD Setting
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
        if (fonts[i].text == oneChar) {
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
            if (string[i] == fonts[j].text) {
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