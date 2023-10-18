/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 09:56:12
 * @LastEditTime: 2023-10-18 17:56:58
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
    store_print_debug();
    // fragment_init();

    wifi_connect();

    ESP_LOGI(APP_TAG, "Run Success!\n");
    while (1) {
        // vTaskDelay(1);
        // fragment_loop();
        delay_ms(1000);
        // printf("wifi state:%d\n", wifi_get_connect_state());
    }
}
