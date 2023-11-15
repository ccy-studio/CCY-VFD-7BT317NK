/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-18 11:43:09
 * @LastEditTime: 2023-10-24 17:21:09
 */
#include "smart_wifi.h"
#include "my_http_server.h"

#define NVS_WIFI_T "wifi_config"
#define NVS_KEY_PWD "pwd"
#define NVS_KEY_SSID "ssid"

static volatile wifi_status_t current_wifi_state;
static volatile u8 retry_count;  // 重试次数
static volatile char ip[50];

void wifi_set_ssid_pwd(char *ssid, char *pwd);

#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"
void wifi_start_smart_server(void);
static void smartconfig_task(void* parm);
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static TaskHandle_t task_thread = NULL;
static EventGroupHandle_t s_wifi_event_group = NULL;
#else

#include "cJSON.h"

void wifi_start_softap();

void wifi_stop_softap();

void wifi_start_softap_server();

static httpd_handle_t ap_server = NULL;
extern const uint8_t ap_html_start[] asm("_binary_ap_html_start");
extern const uint8_t ap_html_end[] asm("_binary_ap_html_end");
static u16 sta_number = 0;
static wifi_ap_record_t *ap_list_buffer = NULL;
#endif

static void event_handler(void *arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
        if (current_wifi_state == WIFI_CONFIGING) {
            wifi_start_smart_server();
            return;
        }
#endif
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // 停止Http后台服务
        http_stop();
        retry_count++;
        if (retry_count > WIFI_RETRY_MAX) {
            // 超过最大重试次数
            current_wifi_state = WIFI_ERROR;
            ESP_LOGI(APP_TAG, "Wifi Retry Max ERROR!");
#if WIFI_CONF_TYPE == WIFI_CONF_AP
            wifi_start_softap();
#elif
            wifi_start_smart_server();
#endif
            return;
        }
        esp_wifi_connect();
#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
#endif
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        memset(ip, 0, sizeof(ip));
        memcpy(ip, ip4addr_ntoa(&event->ip_info.ip), sizeof(ip));
        ESP_LOGI(APP_TAG, "Get ip:[%s]\n", ip);
        retry_count = 0;
        current_wifi_state = WIFI_CONNECTED;
#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
#endif
        // 开启Http后台服务
        http_start();
    }
#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
        else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
            ESP_LOGI(APP_TAG, "Found channel");
        } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
            ESP_LOGI(APP_TAG, "Got SSID and password");
            smartconfig_event_got_ssid_pswd_t* evt =
                (smartconfig_event_got_ssid_pswd_t*)event_data;
            wifi_config_t wifi_config;
            uint8_t ssid[33] = {0};
            uint8_t password[65] = {0};
            uint8_t rvd_data[33] = {0};

            bzero(&wifi_config, sizeof(wifi_config_t));
            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password,
                   sizeof(wifi_config.sta.password));
            wifi_config.sta.bssid_set = evt->bssid_set;

            if (wifi_config.sta.bssid_set == true) {
                memcpy(wifi_config.sta.bssid, evt->bssid,
                       sizeof(wifi_config.sta.bssid));
            }

            memcpy(ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(password, evt->password, sizeof(evt->password));
            ESP_LOGI(APP_TAG, "SSID:%s", ssid);
            ESP_LOGI(APP_TAG, "PASSWORD:%s", password);
            if (evt->type == SC_TYPE_ESPTOUCH_V2) {
                ESP_ERROR_CHECK(
                    esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
                ESP_LOGI(APP_TAG, "RVD_DATA:%s", rvd_data);
            }
            // 保存到NVS
            wifi_set_ssid_pwd((char*)&ssid, (char*)&password);
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
            retry_count = 0;
            ESP_ERROR_CHECK(esp_wifi_connect());
            current_wifi_state = WIFI_CONNECTING;
        }

        else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
            xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
        }
#endif

#if WIFI_CONF_TYPE == WIFI_CONF_AP
    else if (event_id == WIFI_EVENT_AP_START) {
        wifi_start_softap_server();
    } else if (event_id == WIFI_EVENT_AP_STOP) {
    }
#endif
}

void wifi_get_ssid_pwd(char *ssid, char *pwd) {
    nvs_handle_t handle;
    if (nvs_open(NVS_WIFI_T, NVS_READONLY, &handle) != ESP_OK) {
        return;
    }
    u32 size;
    esp_err_t state;
    state = nvs_get_str(handle, NVS_KEY_PWD, NULL, &size);
    if (state != ESP_OK) {
        nvs_close(handle);
        return;
    }
    nvs_get_str(handle, NVS_KEY_PWD, pwd, &size);
    nvs_get_str(handle, NVS_KEY_SSID, NULL, &size);
    nvs_get_str(handle, NVS_KEY_SSID, ssid, &size);
    nvs_close(handle);

    ESP_LOGI(APP_TAG, "NVS Get ssid:%s pwd:%s", ssid, pwd);
}

void wifi_set_ssid_pwd(char *ssid, char *pwd) {
    nvs_handle_t handle;
    if (nvs_open(NVS_WIFI_T, NVS_READWRITE, &handle) != ESP_OK) {
        return;
    }
    nvs_set_str(handle, NVS_KEY_PWD, pwd);
    nvs_set_str(handle, NVS_KEY_SSID, ssid);
    nvs_commit(handle);
    nvs_close(handle);
}

void wifi_init() {
#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
    if (s_wifi_event_group == NULL) {
        s_wifi_event_group = xEventGroupCreate();
#endif
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                               &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(
            IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID,
                                               &event_handler, NULL));
#endif

#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
    }
#endif
}

void wifi_connect(void) {
    wifi_unconnect();
    char ssid[32], pwd[64];
    memset(ssid, 0, sizeof(ssid));
    memset(pwd, 0, sizeof(pwd));
    wifi_get_ssid_pwd(ssid, pwd);
    wifi_init();
    if (strlen(ssid) == 0) {
        // 重新配网
#if WIFI_CONF_TYPE == WIFI_CONF_AP
        wifi_start_softap();
#else
        current_wifi_state = WIFI_CONFIGING;
#endif
    } else {
        current_wifi_state = WIFI_CONNECTING;
        // 连接
        wifi_config_t wifi_config;
        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, ssid, sizeof(ssid));
        memcpy(wifi_config.sta.password, pwd, sizeof(pwd));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    }
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_unconnect(void) {
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler);
#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
    esp_event_handler_unregister(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler);
    if (s_wifi_event_group != NULL) {
        vEventGroupDelete(s_wifi_event_group);
        s_wifi_event_group = NULL;
    }
    if (task_thread != NULL) {
        vTaskDelete(task_thread);
        esp_smartconfig_stop();
        task_thread = NULL;
    }
#endif
    http_stop();
    esp_wifi_deinit();
    current_wifi_state = WIFI_UNCONNECT;
    retry_count = 0;
    memset(ip, 0, sizeof(ip));
}

wifi_status_t wifi_get_connect_state(void) {
    return current_wifi_state;
}

void wifi_erase_config(void) {
    ESP_ERROR_CHECK(nvs_flash_erase())
    ESP_ERROR_CHECK(nvs_flash_init())
}

#if WIFI_CONF_TYPE == WIFI_CONF_ESPTOUCH
void wifi_start_smart_server(void) {
    current_wifi_state = WIFI_CONFIGING;
    if (task_thread != NULL) {
        vTaskDelete(task_thread);
        esp_smartconfig_stop();
    }
    xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    xEventGroupClearBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3,
                &task_thread);
}
static void smartconfig_task(void* parm) {
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_V2));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group,
                                     CONNECTED_BIT | ESPTOUCH_DONE_BIT, true,
                                     false, portMAX_DELAY);

        if (uxBits & CONNECTED_BIT) {
            ESP_LOGI(APP_TAG, "WiFi Connected to ap");
        }

        if (uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(APP_TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
            task_thread = NULL;
        }
    }
}
#else

void wifi_start_softap() {
    esp_wifi_disconnect();
    if (ap_server != NULL) {
        httpd_stop(ap_server);
    }
    if (current_wifi_state == WIFI_CONFIGING) {
        return;
    }
    current_wifi_state = WIFI_CONFIGING;
    wifi_config_t wifi_config = {
            .ap = {.ssid = WIFI_SOFTAP_SSID,
                    .ssid_len = strlen(WIFI_SOFTAP_SSID),
                    .password = "",
                    .max_connection = 1,
                    .authmode = WIFI_AUTH_OPEN},
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
}

void wifi_stop_softap() {
    esp_wifi_disconnect();
    httpd_stop(ap_server);
    ap_server = NULL;
}

void wifi_ap_close_start_sta(void *params) {
    delay_ms(1000);
    wifi_stop_softap();
    char ssid[32], pwd[64];
    memset(ssid, 0, sizeof(ssid));
    memset(pwd, 0, sizeof(pwd));
    wifi_get_ssid_pwd(ssid, pwd);
    wifi_config_t wifi_config = {0};
    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, ssid, sizeof(ssid));
    memcpy(wifi_config.sta.password, pwd, sizeof(pwd));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    retry_count = 0;
    ESP_ERROR_CHECK(esp_wifi_connect());
    current_wifi_state = WIFI_CONNECTING;
    vTaskDelete(NULL);
}

static esp_err_t index_handle(httpd_req_t *req) {
    httpd_resp_set_hdr(req, CONTENT_TYPE_HTML);
    char *str = (char *) malloc(ap_html_end - ap_html_start);
    if (str == NULL) {
        ESP_LOGE(APP_TAG, "Mem Init Fail Err");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    memcpy(str, ap_html_start, ap_html_end - ap_html_start);
    httpd_resp_send(req, str, strlen(str));
    free(str);
    return ESP_OK;
}

static esp_err_t scan_ap_handle(httpd_req_t *req) {
    esp_wifi_scan_stop();
    wifi_scan_config_t scan_config = {0};
    scan_config.scan_time.active.max = 500;  // ms
    esp_wifi_scan_start(&scan_config, true);
    if (ap_list_buffer != NULL) {
        free(ap_list_buffer);
        ap_list_buffer = NULL;
    }
    esp_wifi_scan_get_ap_num(&sta_number);
    ap_list_buffer = malloc(sta_number * sizeof(wifi_ap_record_t));
    if (ap_list_buffer == NULL) {
        ESP_LOGE(APP_TAG, "Failed to malloc buffer to print scan results");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    esp_wifi_scan_get_ap_records(&sta_number,
                                 (wifi_ap_record_t *) ap_list_buffer);

    httpd_resp_set_hdr(req, CONTENT_TYPE_JSON);
    // 构建JSON数据
    char *jsonStr = NULL;
    cJSON *root = cJSON_CreateArray();
    if (root == NULL) {
        goto end;
    }
    for (size_t i = 0; i < sta_number; i++) {
        // ESP_LOGI(APP_TAG, "[%s][rssi=%d]", ap_list_buffer[i].ssid,
        //          ap_list_buffer[i].rssi);
        cJSON *obj = cJSON_CreateObject();
        if (obj == NULL) {
            goto end;
        }
        cJSON_AddStringToObject(obj, "ssid", (char *) ap_list_buffer[i].ssid);
        cJSON_AddNumberToObject(obj, "rssi", ap_list_buffer[i].rssi);
        cJSON_AddItemToArray(root, obj);
    }
    jsonStr = cJSON_PrintUnformatted(root);
    if (jsonStr == NULL) {
        httpd_resp_send_500(req);
        goto end;
    }
    httpd_resp_send(req, jsonStr, strlen(jsonStr));
    free(jsonStr);

    end:
    cJSON_Delete(root);
    if (ap_list_buffer != NULL) {
        free(ap_list_buffer);
        ap_list_buffer = NULL;
    }
    return ESP_OK;
}

static esp_err_t wifi_connect_handle(httpd_req_t *req) {
    httpd_resp_set_hdr(req, CONTENT_TYPE_HTML);
    // 获取请求内的表单数据
    char *buf;
    size_t buf_len;
    char ssid[25] = {0}, pwd[25] = {0};

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(APP_TAG, "Found URL query => %s", buf);
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid)) ==
                ESP_OK) {
                ESP_LOGI(APP_TAG, "Found URL query parameter => ssid=%s", ssid);
            }
            if (httpd_query_key_value(buf, "password", pwd, sizeof(pwd)) ==
                ESP_OK) {
                ESP_LOGI(APP_TAG, "Found URL query parameter => password=%s",
                         pwd);
            }
        }
        free(buf);
    }
    if (strlen(ssid) != 0) {
        // 获取成功进行尝试连接Wi-Fi
        // 保存到NVS
        wifi_set_ssid_pwd(&ssid, &pwd);
        xTaskCreate(wifi_ap_close_start_sta, "WIFI-RSTA", 4096, NULL, 5, NULL);
    }
    char result[84] = "发送Wi-Fi链接信息到设备成功,请转而查看VFD屏幕的WiFi状态变化";
    httpd_resp_send(req, result, strlen(result));
    return ESP_OK;
}

static httpd_uri_t index_t = {.uri = "/",
        .method = HTTP_GET,
        .handler = index_handle,
        .user_ctx = NULL};
static httpd_uri_t scan_ap_t = {.uri = "/scan-ap",
        .method = HTTP_GET,
        .handler = scan_ap_handle,
        .user_ctx = NULL};

static httpd_uri_t wifi_connect_t = {.uri = "/wifi-connect",
        .method = HTTP_GET,
        .handler = wifi_connect_handle,
        .user_ctx = NULL};

void wifi_start_softap_server() {
    if (ap_server != NULL) {
        return;
    }
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_LOGI(APP_TAG, "Starting AP server on port: '%d'", config.server_port);
    if (httpd_start(&ap_server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(APP_TAG, "Registering AP URI handlers");
        httpd_register_uri_handler(ap_server, &index_t);
        httpd_register_uri_handler(ap_server, &scan_ap_t);
        httpd_register_uri_handler(ap_server, &wifi_connect_t);
    } else {
        ESP_LOGI(APP_TAG, "Error starting  AP server!");
    }
}

#endif

char *wifi_get_ip() {
    return (char *) &ip[0];
}