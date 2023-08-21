/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-18 15:43:32
 * @LastEditTime: 2023-08-21 14:32:37
 */
#include <ArduinoJson.h>
#include <store.h>
#include "LittleFS.h"

const static char setting_file[] = "/settings.json";

store_setting_obj setting_obj;

void store_init() {
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
}

void store_close() {
    LittleFS.end();
}

void store_save_setting(store_setting_obj* obj) {
    File file = LittleFS.open(setting_file, "w");

    DynamicJsonDocument jsonDoc(1024);

    // 将结构体数据转换为JSON
    jsonDoc["annoOpen"] = obj->anno_open;
    jsonDoc["rgbOpen"] = obj->rgb_open;
    jsonDoc["rgbStyle"] = obj->rgb_style;
    jsonDoc["rgbBrightness"] = obj->rgb_brightness;
    jsonDoc["customLongText"] = obj->custom_long_text;
    jsonDoc["customLongTextFrame"] = obj->custom_long_text_frame;
    jsonDoc["autoPower"] = obj->auto_power;
    jsonDoc["autoPowerOpenTime"] = obj->auto_power_open_time;
    jsonDoc["autoPowerCloseTime"] = obj->auto_power_close_time;

    JsonArray autoPowerEnableDaysArray =
        jsonDoc.createNestedArray("autoPowerEnableDays");
    for (int i = 0; i < 7; i++) {
        autoPowerEnableDaysArray.add(obj->auto_power_enable_days[i]);
    }

    jsonDoc["alarmClock"] = obj->alarm_clock;
    jsonDoc["alarmClockTime"] = obj->alarm_clock_time;

    JsonArray alarmClockEnableDaysArray =
        jsonDoc.createNestedArray("alarmClockEnableDays");
    for (int i = 0; i < 7; i++) {
        alarmClockEnableDaysArray.add(obj->alarm_clock_enable_days[i]);
    }

    jsonDoc["countdown"] = obj->countdown;
    jsonDoc["countdownTime"] = obj->countdown_time;

    // 将JSON数据发送到客户端
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);

    file.print(jsonResponse);
    file.close();
}

void store_get_setting(store_setting_obj* obj) {
    if (LittleFS.exists(setting_file)) {
        File file = LittleFS.open(setting_file, "r");
        if (!file) {
            Serial.println("Failed to open settings file for reading");
            return;
        }

        String jsonStr = file.readString();

#ifdef DEBUG
        printf("GetStore:%s\n", jsonStr.c_str());
#endif

        DynamicJsonDocument jsonDoc(1024);  // 调整合适的JSON文档大小
        DeserializationError error = deserializeJson(jsonDoc, jsonStr);

        if (error) {
            Serial.println("JSON parsing error!");
            file.close();
            return;
        }

        // 处理JSON数据
        JsonObject root = jsonDoc.as<JsonObject>();
        obj->anno_open = root["annoOpen"].as<u8>();
        obj->rgb_open = root["rgbOpen"].as<u8>();
        obj->rgb_style = root["rgbStyle"].as<u8>();
        obj->rgb_brightness = root["rgbBrightness"].as<u8>();

        strcpy(obj->custom_long_text, root["customLongText"]);
        obj->custom_long_text_frame = root["customLongTextFrame"].as<u8>();

        obj->auto_power = root["autoPower"].as<u8>();
        strcpy(obj->auto_power_open_time, root["autoPowerOpenTime"]);
        strcpy(obj->auto_power_close_time, root["autoPowerCloseTime"]);
        JsonArray arrTmp = root["autoPowerEnableDays"].as<JsonArray>();
        memset(obj->auto_power_enable_days, 0,
               sizeof(obj->auto_power_enable_days));
        for (u8 i = 0; i < arrTmp.size(); i++) {
            obj->auto_power_enable_days[i] = arrTmp[i].as<u8>();
        }

        obj->alarm_clock = root["alarmClock"].as<u8>();
        strcpy(obj->alarm_clock_time, root["alarmClockTime"]);
        arrTmp = root["alarmClockEnableDays"].as<JsonArray>();
        memset(obj->alarm_clock_enable_days, 0,
               sizeof(obj->alarm_clock_enable_days));
        for (u8 i = 0; i < arrTmp.size(); i++) {
            obj->alarm_clock_enable_days[i] = arrTmp[i].as<u8>();
        }

        obj->countdown = root["countdown"].as<u8>();
        strcpy(obj->countdown_time, root["countdownTime"]);

        file.close();
    }
#ifdef DEBUG
    else {
        printf("文件不存在\n");
    }
#endif
}

void store_del_setting(void) {
    if (LittleFS.exists(setting_file)) {
        LittleFS.remove(setting_file);
    }
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