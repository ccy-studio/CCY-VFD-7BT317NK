/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-10 17:55:53
 * @LastEditTime: 2023-10-11 11:31:41
 */
#include <store.h>
#include "nvs_flash.h"

#define MY_PARTITION "mnvs"
#define MY_TABLE "setting_db"

store_setting_obj glob_setting_config;

void store_read_setting();

void store_init() {
    nvs_flash_init_partition(MY_PARTITION);
    glob_setting_config.anno_open = 1;
    glob_setting_config.rgb_open = 1;
    glob_setting_config.rgb_style = 0;
    glob_setting_config.rgb_brightness = 255;
    memcpy(glob_setting_config.custom_long_text, "Hello VFD", 9);
    glob_setting_config.custom_long_text_frame = 255;
    store_read_setting();
}

void store_close() {
    nvs_flash_deinit_partition(MY_PARTITION);
}

store_setting_obj* get_store() {
    return &glob_setting_config;
}

void store_save_setting() {
    nvs_handle my_nvs_handle;
    ESP_ERROR_CHECK(nvs_open_from_partition(MY_PARTITION, MY_TABLE,
                                            NVS_READWRITE, &my_nvs_handle));
    nvs_set_blob(my_nvs_handle, "obj", &glob_setting_config,
                 sizeof(glob_setting_config));
    nvs_commit(my_nvs_handle);
    nvs_close(my_nvs_handle);
}

void store_read_setting() {
    nvs_handle my_nvs_handle;
    if (nvs_open_from_partition(MY_PARTITION, MY_TABLE, NVS_READONLY,
                                &my_nvs_handle) != ESP_OK) {
        return;
    }
    u32 size = 0;
    esp_err_t state = nvs_get_blob(my_nvs_handle, "obj", NULL, &size);
    if (state != ESP_OK) {
        ESP_LOGI(APP_TAG, "nvs read settting_db len fail\n");
        nvs_close(my_nvs_handle);
        return;
    }
    state = nvs_get_blob(my_nvs_handle, "obj", &glob_setting_config, &size);
    if (state != ESP_OK) {
        ESP_LOGI(APP_TAG, "nvs read settting_db fail\n");
        nvs_close(my_nvs_handle);
        return;
    }
    nvs_close(my_nvs_handle);
}

void store_del_setting(void) {
    nvs_flash_erase_partition(MY_PARTITION);
}

#ifdef DEBUG

void store_print_debug() {
    printf("\n输出结果:\n");
    printf("anno_open: %d\n", glob_setting_config.anno_open);
    printf("rgb_open: %d\n", glob_setting_config.rgb_open);
    printf("rgb_style: %d\n", glob_setting_config.rgb_style);
    printf("rgb_brightness: %d\n", glob_setting_config.rgb_brightness);
    printf("custom_long_text: %s\n", glob_setting_config.custom_long_text);
    printf("custom_long_text_frame: %d\n",
           glob_setting_config.custom_long_text_frame);
    printf("auto_power: %d\n", glob_setting_config.auto_power);
    printf("auto_power_open_time: %s\n",
           glob_setting_config.auto_power_open_time);
    printf("auto_power_close_time: %s\n",
           glob_setting_config.auto_power_close_time);

    printf("auto_power_enable_days: ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", glob_setting_config.auto_power_enable_days[i]);
    }
    printf("\n");

    printf("alarm_clock: %d\n", glob_setting_config.alarm_clock);
    printf("alarm_clock_time: %s\n", glob_setting_config.alarm_clock_time);

    printf("alarm_clock_enable_days: ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", glob_setting_config.alarm_clock_enable_days[i]);
    }
    printf("\n");

    printf("countdown: %d\n", glob_setting_config.countdown);
    printf("countdown_time: %s\n", glob_setting_config.countdown_time);
}

#endif