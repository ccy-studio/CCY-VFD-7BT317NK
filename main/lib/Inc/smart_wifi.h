/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-18 11:33:46
 * @LastEditTime: 2023-10-18 16:37:35
 */
#ifndef __SMART_WIFI_H__
#define __SMART_WIFI_H__

#include "constant.h"
#include "esp_event.h"
#include "esp_smartconfig.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "smartconfig_ack.h"
#include "tcpip_adapter.h"

#define WIFI_RETRY_MAX 5

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

void wifi_start_smart_server(void);

char* wifi_get_ip();

#endif