/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系
 *[yustart@foxmail.com] 进行授权。
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
 * @Date: 2023-07-12 14:14:04
 * @LastEditTime: 2023-08-21 23:53:01
 */
#include "gui.h"

u8 lightOff = 1;    // 背光开关
u8 lightLevel = 5;  // 亮度级别

u8 mh1, mh2;  // 冒号1、2

// 记录当前设置单独ICON的值
u32 current_icon_flag = 0;
u32 save_icon = 0;
static u8 long_cancel_flag = 0;

void vfd_gui_init() {
    // 初始化GPIO
    ptInitGPIO();
    pinMode(PWM_PIN, OUTPUT);
    // 设置PWM的频率单位hz
    analogWriteFreq(20000);
    analogWrite(PWM_PIN, 18);
    // VFD Setting
    setDisplayMode(3);  // command1
    vfd_gui_clear();
}

void vfd_gui_stop() {
    vfd_gui_clear();
    current_icon_flag = 0;
    mh1 = 0;
    mh2 = 0;
    digitalWrite(PWM_PIN, LOW);
}

void vfd_gui_clear() {
    setModeWirteDisplayMode(0);  // command2
    u8 clearBuf[24];
    memset(clearBuf, 0x00, sizeof(clearBuf));
    setModeWirteDisplayMode(0);               // command2
    sendDigAndData(0, clearBuf, 24);          // command3
    ptSetDisplayLight(lightOff, lightLevel);  // command4
}

void vfd_gui_set_one_text(size_t index, char oneChar) {
    setModeWirteDisplayMode(0);  // command2
    u32 buf = gui_get_font(oneChar);
    uint8_t arr[3];
    arr[0] = (buf >> 16) & 0xFF;
    arr[1] = (buf >> 8) & 0xFF;
    arr[2] = buf & 0xFF;
    sendDigAndData(index * 3, arr, 3);        // command3
    ptSetDisplayLight(lightOff, lightLevel);  // command4
}

void vfd_gui_set_icon(u32 buf, u8 is_save_state) {
    if (current_icon_flag == buf) {
        // 过滤重复提交
        return;
    }
    uint8_t arr[3];
    arr[0] = (buf >> 16) & 0xFF;
    arr[1] = (buf >> 8) & 0xFF;
    arr[2] = buf & 0xFF;
    setModeWirteDisplayMode(0);               // command2
    sendDigAndData(0, arr, 3);                // command3
    ptSetDisplayLight(lightOff, lightLevel);  // command4
    if (is_save_state) {
        save_icon = buf;
    }
    current_icon_flag = buf;
}

u32 vfd_gui_get_save_icon(void) {
    return save_icon;
}

u8 vfd_gui_set_text(const char* string, const u8 colon) {
    size_t str_len = strlen(string);
    static u8 data[24];
    memset(data, 0, sizeof(data));
    size_t index = 0;
    for (size_t i = 0; i < str_len; i++) {
        if (string[i] && string[i] != '\0') {
            u32 buf = gui_get_font(string[i]);
            data[index++] = (buf >> 16) & 0xFF;
            data[index++] = (buf >> 8) & 0xFF;
            data[index++] = buf & 0xFF;
        }
    }
    // 分号的处理
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
    if (colon) {
        data[5] |= 0x10;
        data[11] |= 0x10;
    }
    setModeWirteDisplayMode(0);               // command2
    sendDigAndData(3, data, 24);              // command3
    ptSetDisplayLight(lightOff, lightLevel);  // command4
    return 1;
}

void vfd_gui_set_bck(u8 onOff) {
    lightOff = onOff;
}

/**
 * 设置亮度等级 1~7
 */
void vfd_gui_set_blk_level(size_t level) {
    lightLevel = level;
}

static void vfd_set_maohao(u8 address, u8 buf) {
    setModeWirteDisplayMode(1);               // command2
    sendDigAndData(address, &buf, 1);         // command3
    ptSetDisplayLight(lightOff, lightLevel);  // command4
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
 * @param loop_count 循环播放的次数
 **/
void vfd_gui_set_long_text(const char* string,
                           u32 delay_ms,
                           size_t loop_count) {
    long_cancel_flag = 0;
    // 刷新的页码数
    size_t pageSize = 1;
    size_t str_len = strlen(string);
    size_t arr_len;
    // 如果大于6满整屏不够显示的
    if (str_len > VFD_DIG_LEN) {
        pageSize += (str_len - VFD_DIG_LEN);
        // pageSize += 5;是渐渐消失需要的页码数量 总数6-1
        pageSize += VFD_DIG_LEN - 1;
    } else {
        // 足够显示或者不满屏幕 str_len - 1;是渐渐消失需要的页码数量
        pageSize += str_len - 1;
    }
    // 正向渐出+动画渐出清屏+反向渐入+反向清屏
    pageSize += loop_count > 1 ? (str_len + VFD_DIG_LEN) : 0;
    arr_len = pageSize * (VFD_DIG_LEN + 1);  //+1是没每一页末尾添加\0
    // 分配内存
    char* buf = (char*)malloc(arr_len * sizeof(char));
    char* zreo_point = buf;
    char* second_point = NULL;
    if (buf == NULL) {
        printf("分配内存失败");
        return;
    }
    // 初始化数组
    memset(buf, ' ', arr_len * sizeof(char));

    // 正向渐出至消失 <-xxxxx
    for (size_t i = 0; i < str_len; i++) {
        memcpy(buf, string + i, VFD_DIG_LEN * sizeof(char));
        buf += VFD_DIG_LEN;
        *buf = '\0';
        buf++;
    }

    // xxxxxx<-
    // 如果循环次数大于1次就初始化动画效果所需要的数据
    if (loop_count > 1) {
        second_point = buf;
        for (size_t i = 0; i < str_len; i++) {
            if (i != 0) {
                memcpy(buf, (buf - VFD_DIG_LEN),
                       (VFD_DIG_LEN - 1) * sizeof(char));
            }
            // 6-1 = 5
            buf += VFD_DIG_LEN - 1;
            *buf = string[i];
            buf++;
            *buf = '\0';
            buf++;
        }
        // 完全渐出
        for (size_t i = 0; i < VFD_DIG_LEN - 1; i++) {
            memcpy(buf, (buf - VFD_DIG_LEN), (VFD_DIG_LEN - 1) * sizeof(char));
            buf += VFD_DIG_LEN - 1;
            *buf = ' ';
            buf++;
            *buf = '\0';
            buf++;
        }
    }

    // 输出
    // 将buf指针位置重新赋值到初始值
    buf = zreo_point;
    for (size_t i = 0; i < pageSize; i++) {
        if (long_cancel_flag) {
            free(zreo_point);
            zreo_point = NULL;
            buf = NULL;
            second_point = NULL;
            return;
        }
        if (vfd_gui_set_text(buf, 0)) {
            delay(delay_ms);
        }
        buf += VFD_DIG_LEN + 1;
    }
    // loop_count大于2的时候开始根据loop_count的数量-2循环到截止.
    if (loop_count > 2 && second_point != NULL) {
        for (size_t i = 0; i < loop_count - 2; i++) {
            buf = second_point;
            while (buf && *buf != '\0') {
                if (long_cancel_flag) {
                    free(zreo_point);
                    zreo_point = NULL;
                    buf = NULL;
                    second_point = NULL;
                    return;
                }
                if (vfd_gui_set_text(buf, 0)) {
                    delay(delay_ms);
                }
                buf += VFD_DIG_LEN + 1;
            }
        }
    }

    // 释放内存空间
    free(zreo_point);
    zreo_point = NULL;
    buf = NULL;
    second_point = NULL;
    vfd_gui_clear();
}

/**
 * G1动画播放
 */
void vfd_gui_anno_for_g1(u8 isAuto) {
    static size_t anno_frame = 0;
    static const u32 anno_arr[] = {
        ICON_G1_STYLE_4, ICON_G1_STYLE_5, ICON_G1_STYLE_6,
        ICON_G1_STYLE_1, ICON_G1_STYLE_2, ICON_G1_STYLE_3,
        ICON_G1_STYLE_7, ICON_G1_STYLE_8, ICON_G1_STYLE_9};
    if (isAuto) {
        anno_frame = 0;
        for (size_t i = 0; i < 9; i++) {
            delay(100);
            u32 anno = anno_arr[i];
            vfd_gui_set_icon(anno | current_icon_flag, 0);
        }
    } else {
        anno_frame++;
        if (anno_frame >= 9) {
            anno_frame = 0;
        }
        u32 anno = anno_arr[anno_frame];
        vfd_gui_set_icon(anno | current_icon_flag, 0);
    }
}

void vfd_gui_cancel_long_text() {
    long_cancel_flag = 1;
}