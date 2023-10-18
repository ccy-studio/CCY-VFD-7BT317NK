/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-18 11:43:09
 * @LastEditTime: 2023-10-18 17:56:01
 */
#include "smart_wifi.h"
#include "my_http_server.h"

#define NVS_WIFI_T "wifi_config"
#define NVS_KEY_PWD "pwd"
#define NVS_KEY_SSID "ssid"

static EventGroupHandle_t s_wifi_event_group = NULL;
static TaskHandle_t task_thread = NULL;
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static wifi_status_t current_wifi_state;
static u8 retry_count;  // 重试次数
static char ip[50];

static void smartconfig_task(void* parm);
void wifi_set_ssid_pwd(char* ssid, char* pwd);

static void event_handler(void* arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        if (current_wifi_state == WIFI_CONFIGING) {
            wifi_start_smart_server();
            return;
        }
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        retry_count++;
        if (retry_count > WIFI_RETRY_MAX) {
            // 超过最大重试次数
            current_wifi_state = WIFI_ERROR;
            ESP_LOGI(APP_TAG, "Wifi Retry Max ERROR!");
            wifi_start_smart_server();
            return;
        }
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        memset(ip, 0, sizeof(ip));
        memcpy(ip, ip4addr_ntoa(&event->ip_info.ip), sizeof(ip));
        ESP_LOGI(APP_TAG, "Get ip:%s\n", ip);
        retry_count = 0;
        current_wifi_state = WIFI_CONNECTED;
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
        //Test
        http_start();
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(APP_TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
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
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
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

void wifi_get_ssid_pwd(char* ssid, char* pwd) {
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

void wifi_set_ssid_pwd(char* ssid, char* pwd) {
    nvs_handle_t handle;
    if (nvs_open(NVS_WIFI_T, NVS_READWRITE, &handle) != ESP_OK) {
        return;
    }
    nvs_set_str(handle, NVS_KEY_PWD, pwd);
    nvs_set_str(handle, NVS_KEY_SSID, ssid);
    nvs_commit(handle);
    nvs_close(handle);
}

esp_err_t wifi_init() {
    if (s_wifi_event_group == NULL) {
        tcpip_adapter_init();
        s_wifi_event_group = xEventGroupCreate();
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                   &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(
            IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID,
                                                   &event_handler, NULL));

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        return ESP_OK;
    }
    return ESP_FAIL;
}
void wifi_connect(void) {
    wifi_unconnect();
    char ssid[32], pwd[64];
    wifi_get_ssid_pwd(ssid, pwd);
    wifi_init();
    if (strlen(ssid) == 0) {
        // 重新配网
        current_wifi_state = WIFI_CONFIGING;
    } else {
        current_wifi_state = WIFI_CONNECTING;
        // 连接
        wifi_config_t wifi_config;
        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, pwd, sizeof(wifi_config.sta.password));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    }
    ESP_ERROR_CHECK(esp_wifi_start());
}
void wifi_unconnect(void) {
    if (s_wifi_event_group != NULL) {
        vEventGroupDelete(s_wifi_event_group);
        s_wifi_event_group = NULL;
    }
    if (task_thread != NULL) {
        vTaskDelete(task_thread);
        esp_smartconfig_stop();
        task_thread = NULL;
    }
    esp_wifi_disconnect();
    esp_wifi_deinit();
    current_wifi_state = WIFI_UNCONNECT;
    retry_count = 0;
    memset(ip, 0, sizeof(ip));
}
wifi_status_t wifi_get_connect_state(void) {
    return current_wifi_state;
}
void wifi_erase_config(void) {
    nvs_handle_t handle;
    if (nvs_open(NVS_WIFI_T, NVS_READWRITE, &handle) != ESP_OK) {
        return;
    }
    nvs_erase_all(handle);
    nvs_close(handle);
}
void wifi_start_smart_server(void) {
    current_wifi_state = WIFI_CONFIGING;
    if (task_thread == NULL) {
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        xEventGroupClearBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
        xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3,
                    &task_thread);
    }
}

char* wifi_get_ip() {
    return &ip;
}