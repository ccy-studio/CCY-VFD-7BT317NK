/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 09:56:12
 * @LastEditTime: 2023-10-11 11:29:41
 */
#include "fragment.h"
#include "driver/uart.h"
#include "esp_system.h"
#include "nvs_flash.h"

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init())
    // 设置Uart 115200
    uart_set_baudrate(UART_NUM_0, 115200);
    printf("RunStarting...\n");
    store_init();
    store_get_setting(&setting_obj);
    store_print_debug(setting_obj);

    fragment_init();
    ESP_LOGI(APP_TAG, "Run Success!\n");
    while (1) {
        delay_ms(1);
        fragment_loop();
    }
}
