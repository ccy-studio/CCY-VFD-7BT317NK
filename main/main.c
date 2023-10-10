/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 09:56:12
 * @LastEditTime: 2023-10-10 16:38:26
 */
#include "nvs_flash.h"
#include "esp_system.h"
#include "fragment.h"
#include "driver/uart.h"

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init())
    // 设置Uart 115200
    uart_set_baudrate(UART_NUM_0, 115200);
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
