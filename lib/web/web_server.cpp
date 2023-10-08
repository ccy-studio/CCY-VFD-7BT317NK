/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系
 *[yustart@foxmail.com] 进行授权。
 *
 * 在遵循以下条件的情况下，您可以自由修改、使用和分发本代码：
 * - 您必须保留此版权声明的所有内容。
 * - 您不得用于任何违法、滥用、恶意的活动。
 * - 您的使用不应损害任何个人、组织或作者的声誉。
 *
 * 作者不承担因使用本代码而产生的任何担保责任。作者对因使用本代码所导致的任何直接或间接损失不承担责任。
 * Github: https://github.com/ccy-studio/CCY-VFD-7BT317NK
 ***********************************************************************************************/

#include <ArduinoJson.h>
#include <web_index_html.h>
#include <web_server.h>

ESP8266WebServer server(80);
WiFiManager wifiManager;

void handle_index();
void handle_404();
void http_reqeust_save_setteing();
void http_reqeust_get_setteing();

void web_setup(ConfigModeCallback modeCallback,
               ConfigTimeOutCallback timeoutCallback) {
    printf("WIFI SSID:%s\n", wifiManager.getWiFiSSID().c_str());
    printf("WIFI PWD:%s\n", wifiManager.getWiFiPass().c_str());

    if (WiFi.isConnected()) {
        return;
    }

    wifiManager.setAPCallback(modeCallback);
    wifiManager.setCountry("CN");
    wifiManager.setBreakAfterConfig(true);
    wifiManager.setTimeout(60);
    // wifiManager.setDebugOutput(false);
    String ssid = "VFD-" + String(ESP.getChipId());
    if (!wifiManager.autoConnect(ssid.c_str(), NULL)) {
        Serial.println("Failed to connect and hit timeout.");
        while (1) {
            timeoutCallback();
        }
    }

    // 配网成功，打印连接信息
    Serial.println("Connected to WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();
    server.on("/", handle_index);
    server.on("/save-setting", http_reqeust_save_setteing);
    server.on("/get-setting", http_reqeust_get_setteing);
    server.onNotFound(handle_404);

    ota_set(&server);
}

void web_stop() {
    ota_close();
    server.close();
    wifiManager.disconnect();
}

void web_loop() {
    if (WiFi.isConnected()) {
        ota_loop();
        server.handleClient();
    }
}

void wifi_erase() {
    wifiManager.erase();
}

void handle_index() {
    String str = FPSTR(HTML_INDEX);
    server.send(200, "text/html; charset=utf-8", str);
}
void handle_404() {
    String html = "<h1>404</h1>";
    server.send(404, "text/html; charset=utf-8", html);
}

void http_reqeust_save_setteing() {
    String body = server.arg("plain");  // 获取POST请求的内容

#ifdef DEBUG
    Serial.println("Received POST data: " + body);
#endif
    // 解析JSON数据
    DynamicJsonDocument jsonDoc(1024);  // 调整合适的JSON文档大小
    DeserializationError error = deserializeJson(jsonDoc, body);

    if (error) {
        Serial.println("JSON parsing error!");
        server.send(400, "text/plain", "Bad Request");
        return;
    }

    // 处理JSON数据
    JsonObject root = jsonDoc.as<JsonObject>();
    setting_obj.anno_open = root["annoOpen"].as<u8>();
    setting_obj.rgb_open = root["rgbOpen"].as<u8>();
    setting_obj.rgb_style = root["rgbStyle"].as<u8>();
    setting_obj.rgb_brightness = root["rgbBrightness"].as<u8>();

    strcpy(setting_obj.custom_long_text, root["customLongText"]);
    setting_obj.custom_long_text_frame = root["customLongTextFrame"].as<u8>();

    setting_obj.auto_power = root["autoPower"].as<u8>();
    strcpy(setting_obj.auto_power_open_time, root["autoPowerOpenTime"]);
    strcpy(setting_obj.auto_power_close_time, root["autoPowerCloseTime"]);
    JsonArray arrTmp = root["autoPowerEnableDays"].as<JsonArray>();
    memset(setting_obj.auto_power_enable_days, 0,
           sizeof(setting_obj.auto_power_enable_days));
    for (u8 i = 0; i < arrTmp.size(); i++) {
        setting_obj.auto_power_enable_days[i] = arrTmp[i].as<u8>();
    }

    setting_obj.alarm_clock = root["alarmClock"].as<u8>();
    strcpy(setting_obj.alarm_clock_time, root["alarmClockTime"]);
    arrTmp = root["alarmClockEnableDays"].as<JsonArray>();
    memset(setting_obj.alarm_clock_enable_days, 0,
           sizeof(setting_obj.alarm_clock_enable_days));
    for (u8 i = 0; i < arrTmp.size(); i++) {
        setting_obj.alarm_clock_enable_days[i] = arrTmp[i].as<u8>();
    }

    setting_obj.countdown = root["countdown"].as<u8>();
    strcpy(setting_obj.countdown_time, root["countdownTime"]);

#ifdef DEBUG
    // 测试打印输出结果
    store_print_debug(setting_obj);
#endif

    // 写数据
    store_save_setting(&setting_obj);

    server.send(200, "application/json", "success");
}

void http_reqeust_get_setteing() {
    // 创建DynamicJsonDocument对象
    DynamicJsonDocument jsonDoc(1024);

    // 将结构体数据转换为JSON
    jsonDoc["annoOpen"] = setting_obj.anno_open ? true : false;
    jsonDoc["rgbOpen"] = setting_obj.rgb_open ? true : false;
    jsonDoc["rgbStyle"] = setting_obj.rgb_style;
    jsonDoc["rgbBrightness"] = setting_obj.rgb_brightness;
    jsonDoc["customLongText"] = setting_obj.custom_long_text;
    jsonDoc["customLongTextFrame"] = setting_obj.custom_long_text_frame;
    jsonDoc["autoPower"] = setting_obj.auto_power ? true : false;
    jsonDoc["autoPowerOpenTime"] = setting_obj.auto_power_open_time;
    jsonDoc["autoPowerCloseTime"] = setting_obj.auto_power_close_time;

    JsonArray autoPowerEnableDaysArray =
        jsonDoc.createNestedArray("autoPowerEnableDays");
    for (int i = 0; i < 7; i++) {
        autoPowerEnableDaysArray.add(setting_obj.auto_power_enable_days[i]);
    }

    jsonDoc["alarmClock"] = setting_obj.alarm_clock ? true : false;
    jsonDoc["alarmClockTime"] = setting_obj.alarm_clock_time;

    JsonArray alarmClockEnableDaysArray =
        jsonDoc.createNestedArray("alarmClockEnableDays");
    for (int i = 0; i < 7; i++) {
        alarmClockEnableDaysArray.add(setting_obj.alarm_clock_enable_days[i]);
    }

    jsonDoc["countdown"] = setting_obj.countdown ? true : false;
    jsonDoc["countdownTime"] = setting_obj.countdown_time;

    // 将JSON数据发送到客户端
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    server.send(200, "application/json", jsonResponse);
}