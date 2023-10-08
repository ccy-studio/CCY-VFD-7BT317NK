#ifndef __OTA_H
#define __OTA_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>
#include <constant.h>

#define OTA_START 1
#define OTA_END 0
#define OTA_ERROR 255

void ota_init();
void ota_loop();
void ota_set(ESP8266WebServer *server);
void ota_close();

#endif