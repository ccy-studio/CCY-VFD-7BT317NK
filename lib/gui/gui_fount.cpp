/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-07-12 15:36:22
 * @LastEditTime: 2023-08-12 14:53:59
 */

/*
    米字字符图片参考：
    https://blog.csdn.net/fcz1116/article/details/124371360
    https://zhuanlan.zhihu.com/p/94972448
*/

#include "gui.h"

/**
 * 下方使用字符生成器生成的，生成器使用Java语言JDK8开发。
 * 工具代码在DOC文件夹下.java文件
 */
const u32 fonts[] = {
    0x202204,  // ASCII:! index:0
    0x300000,  // ASCII:" index:1
    0x505700,  // ASCII:# index:2
    0x2fa70f,  // ASCII:$ index:3
    0x451209,  // ASCII:% index:4
    0x1bca0f,  // ASCII:& index:5
    0x100000,  // ASCII:' index:6
    0x090807,  // ASCII:( index:7
    0x84800e,  // ASCII:) index:8
    0x707700,  // ASCII:* index:9
    0x202700,  // ASCII:+ index:10
    0x008000,  // ASCII:, index:11
    0x000700,  // ASCII:- index:12
    0x000008,  // ASCII:. index:13
    0x441201,  // ASCII:/ index:14
    0xcf980f,  // ASCII:0 index:15
    0x848008,  // ASCII:1 index:16
    0x870f0f,  // ASCII:2 index:17
    0x47870f,  // ASCII:3 index:18
    0x8d8708,  // ASCII:4 index:19
    0x0f870f,  // ASCII:5 index:20
    0x0f8f0f,  // ASCII:6 index:21
    0x8f8008,  // ASCII:7 index:22
    0x8f8f0f,  // ASCII:8 index:23
    0x8f870f,  // ASCII:9 index:24
    0x202000,  // ASCII:: index:25
    0x202001,  // ASCII:; index:26
    0x404100,  // ASCII:< index:27
    0x07000f,  // ASCII:= index:28
    0x101200,  // ASCII:> index:29
    0x472204,  // ASCII:? index:30
    0x8f1f07,  // ASCII:@ index:31
    0x8f8f09,  // ASCII:A index:32
    0xa7a60f,  // ASCII:B index:33
    0x0f080f,  // ASCII:C index:34
    0xa7a20f,  // ASCII:D index:35
    0x0f0f0f,  // ASCII:E index:36
    0x0f0f01,  // ASCII:F index:37
    0x0f8c0f,  // ASCII:G index:38
    0x8d8f09,  // ASCII:H index:39
    0x27220f,  // ASCII:I index:40
    0x222a05,  // ASCII:J index:41
    0x4d4b09,  // ASCII:K index:42
    0x09080f,  // ASCII:L index:43
    0xdd8a09,  // ASCII:M index:44
    0x9dca09,  // ASCII:N index:45
    0x8f880f,  // ASCII:O index:46
    0x8f0f01,  // ASCII:P index:47
    0x8fc80f,  // ASCII:Q index:48
    0x8f4f09,  // ASCII:R index:49
    0x0f870f,  // ASCII:S index:50
    0x272204,  // ASCII:T index:51
    0x8d880f,  // ASCII:U index:52
    0x552200,  // ASCII:V index:53
    0x8dda09,  // ASCII:W index:54
    0x555209,  // ASCII:X index:55
    0x552204,  // ASCII:Y index:56
    0x47120f,  // ASCII:Z index:57
    0x0f080f,  // ASCII:[ index:58
    0x114208,  // ASCII:\ index:59
    0x87800f,  // ASCII:] index:60
    0x205000,  // ASCII:^ index:61
    0x00000f,  // ASCII:_ index:62
    0x100000,  // ASCII:` index:63
};

u32 gui_get_font(char c) {
    if (c == ' ') {
        return 0x00;
    }
    if (c >= 33 && c <= 96) {
        // ! ~ `
        return fonts[map(c, 33, 96, 0, 63)];
    } else if (c >= 97 && c <= 122) {
        //a~z
        return gui_get_font(c - 32);
    } else {
        return 0;
    }
}