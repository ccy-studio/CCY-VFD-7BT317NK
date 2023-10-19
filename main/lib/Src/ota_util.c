#include "ota_util.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_netif.h"
#include "esp_ota_ops.h"
#include "smart_wifi.h"

static ota_status_t current_state = OTA_STATE_NONE;
static ota_app_update_event event_call;
static TaskHandle_t ota_therad = NULL;

esp_err_t _http_event_handler(esp_http_client_event_t* evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ERROR");
            event_call(OTA_STATE_ERROR, NULL);
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ON_CONNECTED");
            event_call(OTA_STATE_CONNECT, NULL);
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s",
                     evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            event_call(OTA_STATE_PROGRESS, evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ON_FINISH");
            event_call(OTA_STATE_FINISH, NULL);
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_DISCONNECTED");
            event_call(OTA_STATE_FINISH, NULL);
            break;
    }
    return ESP_OK;
}

void ota_task_fun(void* params) {
    esp_http_client_config_t config = {
        .url = (char*)params,
        .event_handler = _http_event_handler,
    };
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        esp_restart();
    } else {
        ESP_LOGE(APP_TAG, "Firmware Upgrades Failed");
    }
    while (1) {
        delay_ms(1000);
    }
}

bool ota_update(char* url, ota_app_update_event callback) {
    if (current_state == OTA_STATE_PROGRESS ||
        current_state == OTA_STATE_CONNECT) {
        return false;
    }
    current_state = OTA_STATE_CONNECT;
    event_call = callback;
    xTaskCreate(&ota_task_fun, "ota_task", 8192, url, 5, &ota_therad);
    return true;
}

ota_status_t ota_get_state(){
    return current_state;
}