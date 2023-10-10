#include <store.h>
#include "cJSON.h"
#include "esp_spiffs.h"

const static char setting_file[] = "/spiffs/settings.json";
const static char base_path[] = "/spiffs";

store_setting_obj setting_obj = {
    .anno_open = 1,
    .rgb_open = 1,
    .rgb_style = 2,
    .rgb_brightness = 128,
    .custom_long_text = {"Hello VFD"},
    .custom_long_text_frame = 255
};

void store_init() {
    esp_vfs_spiffs_conf_t conf = {.base_path = base_path,
                                  .partition_label = NULL,
                                  .max_files = 5,
                                  .format_if_mount_failed = true};
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(APP_TAG, "Failed to mount or format filesystem");
            return;
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(APP_TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(APP_TAG, "Failed to initialize SPIFFS (%s)",
                     esp_err_to_name(ret));
            return;
        }
    }

#ifdef DEBUG
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(APP_TAG, "Failed to get SPIFFS partition information (%s)",
                 esp_err_to_name(ret));
    } else {
        ESP_LOGI(APP_TAG, "Partition size: total: %d, used: %d", total, used);
    }
#endif
}

void store_close() {
    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(APP_TAG, "SPIFFS unmounted");
}

void store_save_setting(store_setting_obj* obj) {
    FILE* f = fopen(setting_file, "w");
    if (f == NULL) {
        ESP_LOGE(APP_TAG, "Failed to open file for writing");
        return;
    }

    cJSON* root = cJSON_CreateObject();

    // 将结构体数据转换为JSON
    cJSON_AddNumberToObject(root, "annoOpen", obj->anno_open);
    cJSON_AddNumberToObject(root, "rgbOpen", obj->rgb_open);
    cJSON_AddNumberToObject(root, "rgbStyle", obj->rgb_style);
    cJSON_AddNumberToObject(root, "rgbBrightness", obj->rgb_brightness);
    cJSON_AddStringToObject(root, "customLongText", obj->custom_long_text);
    cJSON_AddNumberToObject(root, "customLongTextFrame",
                            obj->custom_long_text_frame);
    cJSON_AddNumberToObject(root, "autoPower", obj->auto_power);
    cJSON_AddStringToObject(root, "autoPowerOpenTime",
                            obj->auto_power_open_time);
    cJSON_AddStringToObject(root, "autoPowerCloseTime",
                            obj->auto_power_close_time);
    cJSON_AddItemToObject(
        root, "autoPowerEnableDays",
        cJSON_CreateIntArray((const int*)obj->auto_power_enable_days, 7));
    cJSON_AddNumberToObject(root, "alarmClock", obj->alarm_clock);
    cJSON_AddStringToObject(root, "alarmClockTime", obj->alarm_clock_time);
    cJSON_AddItemToObject(
        root, "alarmClockEnableDays",
        cJSON_CreateIntArray((const int*)obj->alarm_clock_enable_days, 7));
    cJSON_AddNumberToObject(root, "countdown", obj->countdown);
    cJSON_AddStringToObject(root, "countdownTime", obj->countdown_time);

    char* jsonStr = cJSON_Print(root);

    printf("JSONSave Suscess:%s\n", jsonStr);

    fprintf(f, jsonStr);
    fflush(f);
    fclose(f);
    free(jsonStr);
    cJSON_Delete(root);
    free(root);
}

void store_get_setting(store_setting_obj* obj) {
    FILE* f = fopen(setting_file, "r");
    if (f == NULL) {
        ESP_LOGE(APP_TAG, "Failed to open file for writing");
        return;
    }
    char jsonStr[1024];
    fgets(jsonStr, sizeof(jsonStr), f);
    fclose(f);

#ifdef DEBUG
    printf("GetStore:%s\n", jsonStr);
#endif
    cJSON* json_obj = cJSON_Parse(jsonStr);
    if (json_obj == NULL) {
        ESP_LOGE(APP_TAG, "JsonParse Error");
        return;
    }
    const cJSON* resolution = NULL;
    const cJSON* resolutions = NULL;
    size_t i = 0;
    // 处理JSON数据
    obj->anno_open =
        cJSON_GetObjectItemCaseSensitive(json_obj, "annoOpen")->valueint;
    obj->rgb_style =
        cJSON_GetObjectItemCaseSensitive(json_obj, "rgbStyle")->valueint;
    obj->rgb_brightness =
        cJSON_GetObjectItemCaseSensitive(json_obj, "rgbBrightness")->valueint;
    strcpy(obj->custom_long_text,
           cJSON_GetObjectItemCaseSensitive(json_obj, "customLongText")
               ->valuestring);
    obj->custom_long_text_frame =
        cJSON_GetObjectItemCaseSensitive(json_obj, "customLongTextFrame")
            ->valueint;
    obj->auto_power =
        cJSON_GetObjectItemCaseSensitive(json_obj, "autoPower")->valueint;
    strcpy(obj->auto_power_open_time,
           cJSON_GetObjectItemCaseSensitive(json_obj, "autoPowerOpenTime")
               ->valuestring);
    strcpy(obj->auto_power_close_time,
           cJSON_GetObjectItemCaseSensitive(json_obj, "autoPowerCloseTime")
               ->valuestring);

    resolutions =
        cJSON_GetObjectItemCaseSensitive(json_obj, "autoPowerEnableDays");
    memset(obj->auto_power_enable_days, 0, sizeof(obj->auto_power_enable_days));
    cJSON_ArrayForEach(resolution, resolutions) {
        obj->auto_power_enable_days[i++] = resolution->valueint;
    }
    obj->alarm_clock =
        cJSON_GetObjectItemCaseSensitive(json_obj, "alarmClock")->valueint;
    strcpy(obj->alarm_clock_time,
           cJSON_GetObjectItemCaseSensitive(json_obj, "alarmClockTime")
               ->valuestring);

    resolutions =
        cJSON_GetObjectItemCaseSensitive(json_obj, "alarmClockEnableDays");
    memset(obj->alarm_clock_enable_days, 0,
           sizeof(obj->alarm_clock_enable_days));
    i = 0;
    cJSON_ArrayForEach(resolution, resolutions) {
        obj->alarm_clock_enable_days[i++] = resolution->valueint;
    }

    obj->countdown =
        cJSON_GetObjectItemCaseSensitive(json_obj, "countdown")->valueint;
    strcpy(obj->countdown_time,
           cJSON_GetObjectItemCaseSensitive(json_obj, "countdownTime")
               ->valuestring);
}

void store_del_setting(void) {}

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