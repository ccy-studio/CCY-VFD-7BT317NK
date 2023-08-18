#include <FS.h>
#include <store.h>

const static char setting_file[] = "settings";

store_setting_obj setting_obj;

void store_save_setting(store_setting_obj obj) {
    SPIFFS.begin();
    File file = SPIFFS.open(setting_file, "w");
    if (!file) {
        Serial.println("Failed to open settings file for writing");
        return;
    }
    file.write((uint8_t*)&obj, sizeof(obj));
    file.close();
    SPIFFS.end();
}

void store_get_setting(store_setting_obj* obj) {
    SPIFFS.begin();
    if (SPIFFS.exists(setting_file)) {
        File file = SPIFFS.open(setting_file, "r");
        if (!file) {
            Serial.println("Failed to open settings file for reading");
            return;
        }

        file.read((uint8_t*)&obj, sizeof(obj));
        file.close();
    }
    SPIFFS.end();
}

void store_del_setting(void) {
    SPIFFS.begin();
    if (SPIFFS.exists(setting_file)) {
        SPIFFS.remove(setting_file);
    }
    SPIFFS.end();
}

void store_print_debug(store_setting_obj setting_obj) {
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