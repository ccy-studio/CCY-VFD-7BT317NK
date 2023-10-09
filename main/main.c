/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 09:56:12
 * @LastEditTime: 2023-10-09 17:10:45
 */
#include "gui.h"
#include "driver/uart.h"
#include "ws2812b.h"

void app_main() {
    //设置Uart 115200
    uart_set_baudrate(UART_NUM_0, 115200);
    vfd_gui_init();
    vfd_gui_start();
    rbg_init();

    delay_ms(1000);
    vfd_gui_set_text("Hello", 1);
    delay_ms(1000);
    vfd_gui_set_text("VFDT", 0);
    u8 hue = 0;
    while (1) {
        rgb_t res = rgb_heat_color(hue);
        rgb_set_color(0, res.r, res.g, res.b);
        rgb_set_color(1, res.r, res.g, res.b);
        hue += 10;
        rgb_update();
        delay_ms(5);
    }
}
