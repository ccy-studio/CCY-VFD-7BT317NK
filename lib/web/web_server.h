#ifndef __WEB_SERVER_H
#define __WEB_SERVER_H

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <store.h>

typedef void (*ConfigModeCallback)(WiFiManager* myWiFiManager);
typedef void (*ConfigTimeOutCallback)();

void web_setup(ConfigModeCallback modeCallback,ConfigTimeOutCallback timeoutCallback);

void web_stop();

void web_loop();

void wifi_erase();

#endif