/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-18 16:40:24
 * @LastEditTime: 2023-10-18 16:41:44
 */
#ifndef _HTTP_SER_H
#define _HTTP_SER_H

#include "constant.h"
#include "esp_netif.h"
#include <esp_http_server.h>

#define CONTENT_TYPE_HTML "Content-Type", "text/html; charset=utf-8"
#define CONTENT_TYPE_JSON "Content-Type", "application/json"

void http_start();
void http_stop();

#endif