/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-10 17:55:53
 * @LastEditTime: 2023-10-11 11:31:41
 */
#include <store.h>
#include "nvs_flash.h"

store_setting_obj setting_obj = {.anno_open = 1,
                                 .rgb_open = 1,
                                 .rgb_style = 2,
                                 .rgb_brightness = 128,
                                 .custom_long_text = {"Hello VFD"},
                                 .custom_long_text_frame = 255};

#define MY_PARTITION "mnvs"
#define MY_TABLE "setting_db"

void store_init() {
    nvs_flash_init_partition(MY_PARTITION);
}

void store_close() {
    nvs_flash_deinit_partition(MY_PARTITION);
}

void store_save_setting(store_setting_obj obj) {
    nvs_handle my_nvs_handle;
    ESP_ERROR_CHECK(nvs_open_from_partition(MY_PARTITION, MY_TABLE,
                                            NVS_READWRITE, &my_nvs_handle));
    nvs_set_blob(my_nvs_handle, "obj", &obj, sizeof(obj));
    nvs_commit(my_nvs_handle);
    nvs_close(my_nvs_handle);
}

void store_get_setting(store_setting_obj* obj) {
    nvs_handle my_nvs_handle;
    if (nvs_open_from_partition(MY_PARTITION, MY_TABLE, NVS_READONLY,
                                &my_nvs_handle) != ESP_OK) {
        return;
    }
    u32 size = 0;
    esp_err_t state = nvs_get_blob(my_nvs_handle, "obj", NULL, &size);
    if (state != ESP_OK) {
        ESP_LOGI(APP_TAG, "nvs read settting_db len fail\n");
        return;
    }
    state = nvs_get_blob(my_nvs_handle, "obj", obj, &size);
    if (state != ESP_OK) {
        ESP_LOGI(APP_TAG, "nvs read settting_db fail\n");
        return;
    }
}

void store_del_setting(void) {
    nvs_flash_erase_partition(MY_PARTITION);
}

#ifdef DEBUG
void store_print_debug(store_setting_obj setting_obj) {
    printf("\n输出结果:\n");
    printf("anno_open: %d\n", setting_obj.anno_open);
    printf("rgb_open: %d\n", setting_obj.rgb_open);
    printf("rgb_style: %d\n", setting_obj.rgb_style);
    printf("rgb_brightness: %d\n", setting_obj.rgb_brightness);
    printf("custom_long_text: %s\n", setting_obj.custom_long_text);
    printf("custom_long_text_frame: %d\n", setting_obj.custom_long_text_frame);
    printf("auto_power: %d\n", setting_obj.auto_power);
    printf("auto_power_open_time: %s\n", setting_obj.auto_power_open_time);
    printf("auto_power_close_time: %s\n", setting_obj.auto_power_close_time);

    printf("auto_power_enable_days: ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", setting_obj.auto_power_enable_days[i]);
    }
    printf("\n");

    printf("alarm_clock: %d\n", setting_obj.alarm_clock);
    printf("alarm_clock_time: %s\n", setting_obj.alarm_clock_time);

    printf("alarm_clock_enable_days: ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", setting_obj.alarm_clock_enable_days[i]);
    }
    printf("\n");

    printf("countdown: %d\n", setting_obj.countdown);
    printf("countdown_time: %s\n", setting_obj.countdown_time);
}
#endif