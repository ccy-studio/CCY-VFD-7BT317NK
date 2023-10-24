/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-18 11:33:46
 * @LastEditTime: 2023-10-24 17:22:13
 */
#ifndef __SMART_WIFI_H__
#define __SMART_WIFI_H__

//Wi-Fi配网模式 
//WIFI_CONF_ESPTOUCH ESPTOUCH工具配网需要下载APP
//WIFI_CONF_AP AP热点配网需要连接热点打开192.168.4.1按照操作配置
#define WIFI_CONF_ESPTOUCH 0x01
#define WIFI_CONF_AP 0x02
#define WIFI_CONF_TYPE WIFI_CONF_AP

// AP热点开启后默认IP为: http://192.168.4.1/
#define WIFI_SOFTAP_SSID "VFD-AP"
#define WIFI_RETRY_MAX 2

#include "constant.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

typedef enum {
    WIFI_UNCONNECT = 0,
    WIFI_CONNECTED,
    WIFI_CONNECTING,
    WIFI_CONFIGING,
    WIFI_ERROR,
} wifi_status_t;

void wifi_connect(void);

void wifi_unconnect(void);

wifi_status_t wifi_get_connect_state(void);

void wifi_erase_config(void);

char* wifi_get_ip();

#endif