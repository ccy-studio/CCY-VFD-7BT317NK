/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-19 15:39:06
 * @LastEditTime: 2023-10-19 15:51:48
 */
#ifndef _HOTA_U
#define _HOTA_U

#include "constant.h"

#define OTA_APP_NAME "vfd-rtos"
#define OTA_CURRENT_VERSION "rtos-3.1"
#define OTA_CURRENT_VERSION_N 31

typedef enum {
    OTA_STATE_CONNECT = 0,
    OTA_STATE_FINISH,
    OTA_STATE_ERROR,
    OTA_STATE_PROGRESS,
    OTA_STATE_NONE,
} ota_status_t;

typedef void (*ota_app_update_event)(ota_status_t state, void* params);

bool ota_update(char* url, ota_app_update_event callback);
ota_status_t ota_get_state();
#endif