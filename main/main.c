/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 09:56:12
 * @LastEditTime: 2023-10-08 17:58:53
 */
#include "constant.h"
#include "gui.h"
#include "uart.h"

void app_main() {
    //设置Uart 115200
    uart_set_baudrate(UART_NUM_0,115200);
    vfd_gui_init();
    vfd_gui_start();
    while (1) {
        printf("Hello Worold!\n");
        delay_ms(500);
    }
}
