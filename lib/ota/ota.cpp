/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-24 09:30:45
 * @LastEditTime: 2023-08-24 10:37:02
 */
#include <ota.h>
ESP8266HTTPUpdateServer httpUpdater;

void ota_init() {
    ArduinoOTA.begin();
}

void ota_set(ESP8266WebServer* server) {
    httpUpdater.setup(server);
}

void ota_loop() {
    ArduinoOTA.handle();
}

void ota_close() {
    ArduinoOTA.end();
}