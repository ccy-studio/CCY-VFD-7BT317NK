/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-18 16:44:26
 * @LastEditTime: 2023-10-19 17:33:17
 */
#include "cJSON.h"
#include "my_http_server.h"
#include "ota_util.h"
#include "smart_wifi.h"
#include "store.h"

#define CONTENT_TYPE_HTML "Content-Type", "text/html; charset=utf-8"
#define CONTENT_TYPE_JSON "Content-Type", "application/json"
static const char* RSP_OK = "success";

static httpd_handle_t server = NULL;

// HTML压缩工具:https://www.lddgo.net/string/html-beautify-minify
// https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-guides/build-system.html
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

u8 cJSON_GetU8(cJSON* node, const char* key);
void cJSON_GetStr(cJSON* node, const char* key, char* buf);
void cJSON_GetU8Arr(cJSON* node, const char* key, u8* arr, u8 len);
void ota_event_handle(ota_status_t state, void* params);

// Method
esp_err_t index_handle(httpd_req_t* req) {
    httpd_resp_set_hdr(req, CONTENT_TYPE_HTML);
    char* str = (char*)malloc(index_html_end - index_html_start);
    if (str == NULL) {
        ESP_LOGE(APP_TAG, "Mem Init Fail Err");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    memcpy(str, index_html_start, index_html_end - index_html_start);
    httpd_resp_send(req, str, strlen(str));
    free(str);
    return ESP_OK;
}

esp_err_t get_setting_handle(httpd_req_t* req) {
    httpd_resp_set_hdr(req, CONTENT_TYPE_JSON);
    // cJson版本不低于v1.7.16否则报错
    char* jsonStr = NULL;
    cJSON* root = cJSON_CreateObject();
    cJSON* array = NULL;
    if (root == NULL) {
        goto end;
    }
    cJSON_AddBoolToObject(root, "annoOpen", glob_setting_config.anno_open);
    cJSON_AddBoolToObject(root, "rgbOpen", glob_setting_config.rgb_open);
    cJSON_AddNumberToObject(root, "rgbStyle", glob_setting_config.rgb_style);
    cJSON_AddNumberToObject(root, "rgbBrightness",
                            glob_setting_config.rgb_brightness);
    cJSON_AddStringToObject(root, "customLongText",
                            glob_setting_config.custom_long_text);
    cJSON_AddNumberToObject(root, "customLongTextFrame",
                            glob_setting_config.custom_long_text_frame);
    cJSON_AddBoolToObject(root, "autoPower", glob_setting_config.auto_power);
    cJSON_AddStringToObject(root, "autoPowerOpenTime",
                            glob_setting_config.auto_power_open_time);
    cJSON_AddStringToObject(root, "autoPowerCloseTime",
                            glob_setting_config.auto_power_close_time);

    array = cJSON_AddArrayToObject(root, "autoPowerEnableDays");
    for (size_t i = 0; i < 7; i++) {
        cJSON_AddItemToArray(
            array,
            cJSON_CreateNumber(glob_setting_config.auto_power_enable_days[i]));
    }

    cJSON_AddBoolToObject(root, "alarmClock", glob_setting_config.alarm_clock);
    cJSON_AddStringToObject(root, "alarmClockTime",
                            glob_setting_config.alarm_clock_time);
    array = cJSON_AddArrayToObject(root, "alarmClockEnableDays");
    for (size_t i = 0; i < 7; i++) {
        cJSON_AddItemToArray(
            array,
            cJSON_CreateNumber(glob_setting_config.alarm_clock_enable_days[i]));
    }
    cJSON_AddBoolToObject(root, "countdown", glob_setting_config.countdown);
    cJSON_AddStringToObject(root, "countdownTime",
                            glob_setting_config.countdown_time);
    jsonStr = cJSON_PrintUnformatted(root);
    if (jsonStr == NULL) {
        httpd_resp_send_500(req);
        goto end;
    }
    httpd_resp_send(req, jsonStr, strlen(jsonStr));
end:
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t save_setting_handle(httpd_req_t* req) {
    httpd_resp_set_hdr(req, CONTENT_TYPE_JSON);
    int ret, remaining = req->content_len;
    char buf[remaining + 1];
    if ((ret = httpd_req_recv(req, buf, remaining)) <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    cJSON* root = cJSON_Parse(buf);
    if (root == NULL) {
        httpd_resp_send_500(req);
        goto end;
    }

    glob_setting_config.anno_open = cJSON_GetU8(root, "annoOpen");
    glob_setting_config.rgb_open = cJSON_GetU8(root, "rgbOpen");
    glob_setting_config.rgb_style = cJSON_GetU8(root, "rgbStyle");
    glob_setting_config.rgb_brightness = cJSON_GetU8(root, "rgbBrightness");

    cJSON_GetStr(root, "customLongText", glob_setting_config.custom_long_text);
    glob_setting_config.custom_long_text_frame =
        cJSON_GetObjectItemCaseSensitive(root, "customLongTextFrame")->valueint;
    glob_setting_config.auto_power = cJSON_GetU8(root, "autoPower");
    cJSON_GetStr(root, "autoPowerOpenTime",
                 glob_setting_config.auto_power_open_time);
    cJSON_GetStr(root, "autoPowerCloseTime",
                 glob_setting_config.auto_power_close_time);
    cJSON_GetU8Arr(root, "autoPowerEnableDays",
                   glob_setting_config.auto_power_enable_days, 7);

    glob_setting_config.alarm_clock = cJSON_GetU8(root, "alarmClock");
    cJSON_GetStr(root, "alarmClockTime", glob_setting_config.alarm_clock_time);
    cJSON_GetU8Arr(root, "alarmClockEnableDays",
                   glob_setting_config.alarm_clock_enable_days, 7);

    glob_setting_config.countdown = cJSON_GetU8(root, "countdown");
    cJSON_GetStr(root, "countdownTime", glob_setting_config.countdown_time);
    httpd_resp_send(req, RSP_OK, strlen(RSP_OK));
end:
    cJSON_Delete(root);
    store_save_setting();
    return ESP_OK;
}

esp_err_t get_app_info_handle(httpd_req_t* req) {
    httpd_resp_set_hdr(req, CONTENT_TYPE_JSON);
    // cJson版本不低于v1.7.16否则报错
    char* jsonStr = NULL;
    cJSON* root = cJSON_CreateObject();
    if (root == NULL) {
        goto end;
    }
    cJSON_AddStringToObject(root, "appName", OTA_APP_NAME);
    cJSON_AddStringToObject(root, "appVersion", OTA_CURRENT_VERSION);
    cJSON_AddNumberToObject(root, "appVersionCode", OTA_CURRENT_VERSION_N);
    jsonStr = cJSON_PrintUnformatted(root);
    if (jsonStr == NULL) {
        httpd_resp_send_500(req);
        goto end;
    }
    httpd_resp_send(req, jsonStr, strlen(jsonStr));
end:
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t receive_ota_url_handle(httpd_req_t* req) {
    httpd_resp_set_hdr(req, CONTENT_TYPE_JSON);
    int ret, remaining = req->content_len;
    char buf[remaining + 1];
    if ((ret = httpd_req_recv(req, buf, remaining)) <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    cJSON* root = cJSON_Parse(buf);
    if (root == NULL) {
        httpd_resp_send_500(req);
        goto end;
    }
    char url_buf[255];
    memset(url_buf, 0, sizeof(url_buf));
    cJSON_GetStr(root, "url", url_buf);
    ESP_LOGI(APP_TAG, "OTA Url:%s", url_buf);
    ota_update(url_buf, ota_event_handle);
    httpd_resp_send(req, RSP_OK, strlen(RSP_OK));
end:
    cJSON_Delete(root);
    return ESP_OK;
    return ESP_OK;
}

// URL
httpd_uri_t index_t = {.uri = "/",
                       .method = HTTP_GET,
                       .handler = index_handle,
                       .user_ctx = NULL};
httpd_uri_t get_setting_t = {.uri = "/get-setting",
                             .method = HTTP_GET,
                             .handler = get_setting_handle,
                             .user_ctx = NULL};

httpd_uri_t save_setting_t = {.uri = "/save-setting",
                              .method = HTTP_POST,
                              .handler = save_setting_handle,
                              .user_ctx = NULL};

httpd_uri_t app_info_t = {.uri = "/appinfo",
                          .method = HTTP_GET,
                          .handler = get_app_info_handle,
                          .user_ctx = NULL};

httpd_uri_t ota_update_t = {.uri = "/ota-update",
                            .method = HTTP_POST,
                            .handler = receive_ota_url_handle,
                            .user_ctx = NULL};

void http_start() {
    if (server != NULL) {
        httpd_stop(server);
    }
    if (wifi_get_connect_state() == WIFI_CONNECTED) {
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        ESP_LOGI(APP_TAG, "Starting server on port: '%d'", config.server_port);
        if (httpd_start(&server, &config) == ESP_OK) {
            // Set URI handlers
            ESP_LOGI(APP_TAG, "Registering URI handlers");
            httpd_register_uri_handler(server, &index_t);
            httpd_register_uri_handler(server, &get_setting_t);
            httpd_register_uri_handler(server, &save_setting_t);
            httpd_register_uri_handler(server, &app_info_t);
            httpd_register_uri_handler(server, &ota_update_t);
        } else {
            ESP_LOGI(APP_TAG, "Error starting server!");
        }
    }
}
void http_stop() {
    if (server != NULL) {
        httpd_stop(server);
    }
}

u8 cJSON_GetU8(cJSON* node, const char* key) {
    cJSON* cj = cJSON_GetObjectItemCaseSensitive(node, key);
    return cj->valueint;
}

void cJSON_GetStr(cJSON* node, const char* key, char* buf) {
    char* str =
        cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(node, key));
    if (str == NULL) {
        return;
    }
    memset(buf, 0, strlen(buf));
    memcpy(buf, str, strlen(str));
}

void cJSON_GetU8Arr(cJSON* node, const char* key, u8* arr, u8 len) {
    cJSON* cj = cJSON_GetObjectItemCaseSensitive(node, key);
    int strLen = cJSON_GetArraySize(cj);
    memset(arr, 0, sizeof(u8) * len);
    for (size_t i = 0; i < strLen; i++) {
        arr[i] = cJSON_GetArrayItem(cj, i)->valueint;
    }
}

void ota_event_handle(ota_status_t state, void* params) {}