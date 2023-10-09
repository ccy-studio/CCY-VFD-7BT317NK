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
    delay_ms(1000);
    vfd_gui_set_text("Hello", 1);
    delay_ms(1000);
    vfd_gui_set_text("VFDT", 0);
    while (1) {
        ESP_LOGI("main.c", "hello!\n");
        delay_ms(500);
        vfd_gui_set_text("VFDT", 0);
    }
}
