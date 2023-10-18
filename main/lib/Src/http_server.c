#include "my_http_server.h"
#include "smart_wifi.h"

#define CONTENT_TYPE_HTML "Content-Type", "text/html; charset=utf-8"

static httpd_handle_t server = NULL;

//https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-guides/build-system.html
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

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

// URL
httpd_uri_t index_t = {.uri = "/",
                       .method = HTTP_GET,
                       .handler = index_handle,
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
            // httpd_register_uri_handler(server, &echo);
            // httpd_register_uri_handler(server, &ctrl);
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