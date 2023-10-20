/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-10 17:55:53
 * @LastEditTime: 2023-10-20 13:54:58
 */

#include "my_util.h"
#include "lwip/apps/sntp.h"

#define NTP_SERVER_0 "ntp.aliyun.com"
#define NTP_SERVER_1 "time1.cloud.tencent.com"

#define NTP_DONE BIT0

static TaskHandle_t ntp_thread = NULL;
static EventGroupHandle_t ntp_event = NULL;
static struct tm timeinfo;

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    const long dividend = out_max - out_min;
    const long divisor = in_max - in_min;
    const long delta = x - in_min;

    return (delta * dividend + (divisor / 2)) / divisor + out_min;
}

static void ntp_sync_time_cb(void* params) {
    // wait for time to be set
    time_t now = 0;
    memset(&timeinfo, 0, sizeof(struct tm));
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(APP_TAG, "Waiting for system time to be set... (%d/%d)", retry,
                 retry_count);
        delay_ms(1500);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    setenv("TZ", "CST-8", 1);
    tzset();
    time(&now);
    localtime_r(&now, &timeinfo);
    xEventGroupSetBits(ntp_event, NTP_DONE);
    vTaskDelete(NULL);
}

esp_time* ntp_async() {
    if (sntp_enabled()) {
        sntp_stop();
    }
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, NTP_SERVER_0);
    sntp_setservername(1, NTP_SERVER_1);
    if (ntp_thread != NULL) {
        return NULL;
    }
    if (ntp_event == NULL) {
        ntp_event = xEventGroupCreate();
    }
    xEventGroupClearBits(ntp_event, NTP_DONE);
    xTaskCreate(ntp_sync_time_cb, "ntp", 4096, NULL, 5, &ntp_thread);
    sntp_init();
    xEventGroupWaitBits(ntp_event, NTP_DONE, true, false, portMAX_DELAY);
    ntp_thread = NULL;
    return &timeinfo;
}