/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-07-28 10:26:44
 * @LastEditTime: 2023-09-25 21:45:36
 */
#include <Arduino.h>
#include <fragment.h>

void setup() {
    Serial.begin(115200);

    // 初始化StoreFS
    store_init();
    // 读数据
    store_get_setting(&setting_obj);
    delay(200);
    fragment_init();
    printf("Run Success!\n");
}

void loop() {
    delay(1);
    fragment_loop();
}
