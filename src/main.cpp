/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-07-28 10:26:44
 * @LastEditTime: 2023-09-24 01:08:46
 */
#include <Arduino.h>
#include <fragment.h>

void setup() {
    Serial.begin(115200);
    // 初始化StoreFS
    store_init();
    // 读数据
    store_get_setting(&setting_obj);
    fragment_init();
}

void loop() {
    fragment_loop();
}
