/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 10:04:30
 * @LastEditTime: 2023-10-20 10:44:46
 */

#ifndef __CONSTANT_H
#define __CONSTANT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#include "esp_system.h"
#include "stdbool.h"

// 是否开启debug模式打印日志,不需要请注释掉
#define DEBUG

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define IO_MASK(pin) 1ULL << pin
#define delay_ms(ms) vTaskDelay(pdMS_TO_TICKS(ms))
#define delay_us(us) ets_delay_us(us)

#define APP_TAG "application"
#define CHECK_LOGE(x, msg, ...)                    \
    do {                                           \
        esp_err_t __;                              \
        if ((__ = x) != ESP_OK) {                  \
            ESP_LOGE(APP_TAG, msg, ##__VA_ARGS__); \
            return __;                             \
        }                                          \
    } while (0)

#define CHECK_ARG(VAL)                  \
    do {                                \
        if (!(VAL))                     \
            return ESP_ERR_INVALID_ARG; \
    } while (0)

#define PIN_INIT_NONE_OUTPUT(pin)           \
    do {                                    \
        gpio_config_t gpio;                 \
        gpio.mode = GPIO_MODE_OUTPUT;       \
        gpio.pull_down_en = 0;              \
        gpio.pull_down_en = 0;              \
        gpio.intr_type = GPIO_INTR_DISABLE; \
        gpio.pin_bit_mask = IO_MASK(pin);   \
        gpio_config(&gpio);                 \
    } while (0)

typedef void (*fun_key_action)(u8 start);

// 按键PIN脚
#define KEY1 5
#define KEY2 4
#define KEY3 2

#define I2C_SDA 4
#define I2C_SCL 5

/**
 * G1动画帧率
 */
#define G1_ANNO_FRAME 150

/**
 * RGB帧率
 */
#define RGB_ANNO_FRAME 5

/**
 * VFD时间刷新帧率
 */
#define VFD_TIME_FRAME 500

#endif