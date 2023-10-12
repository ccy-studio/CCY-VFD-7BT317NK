/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 11:07:12
 * @LastEditTime: 2023-10-12 18:10:57
 */
#include "button.h"
#include "esp_log.h"

static button_t* buttons[BTN_MAX] = {NULL};
QueueHandle_t btn_queue;

static void scan_btn(button_t* btn) {
    if (btn->internal.press_time >= BTN_LONG_PRESS_MS &&
        btn->state == BUTTON_PRESSED) {
        btn->internal.press_time += BTN_SCAN_MS;
        return;
    }
    if (gpio_get_level(btn->gpio) == BTN_PRESS_LEVEL) {
        // press
        if (btn->state == BUTTON_PRESSED) {
            btn->internal.press_time += BTN_SCAN_MS;
        } else if (btn->state == BUTTON_RELEASED) {
            btn->state = BUTTON_PRESSED;
            btn->internal.press_time = 0;
            // callback
            xQueueSend(btn_queue, btn, 0);
            return;
        }
        if (btn->state == BUTTON_PRESSED &&
            btn->internal.press_time >= BTN_LONG_PRESS_MS) {
            // long press
            btn->state = BUTTON_PRESSED_LONG;
            btn->internal.press_time += BTN_SCAN_MS;
            xQueueSend(btn_queue, btn, 0);
            return;
        }
    } else {
        // release
        if (btn->state == BUTTON_PRESSED) {
            if (btn->internal.d_count + 1 == 2 &&
                btn->internal.d_time <= BTN_LONG_DOUBLE_MS) {
                // double
                btn->state = BUTTON_DOUBLE_PRESS;
                btn->internal.d_count = 0;
                btn->internal.d_time = 0;
                xQueueSend(btn_queue, btn, 0);
                return;
            }
            btn->state = BUTTON_RELEASED;
            btn->internal.press_time = 0;
            btn->internal.d_count += 1;
            btn->internal.d_time += BTN_SCAN_MS;
            xQueueSend(btn_queue, btn, 0);
        } else {
            btn->state = BUTTON_RELEASED;
            btn->internal.press_time = 0;
        }
    }
}

void button_task_run(void* params) {
    for (size_t i = 0; i < BTN_MAX; i++) {
        if (buttons[i] != NULL) {
            scan_btn(buttons[i]);
        }
    }
}

void button_init() {
    btn_queue = xQueueCreate(5, sizeof(button_t));
    xTaskCreate(button_task_run, "SCAN", 1024, NULL, 5, NULL);
}
void button_add_btn(button_t* button) {
    for (size_t i = 0; i < BTN_MAX; i++) {
        if (buttons[i] == button) {
            break;
        }
        if (!buttons[i]) {
            vTaskSuspendAll();
            button->state = BUTTON_RELEASED;
            button->internal.press_time = 0;
            button->internal.d_count = 0;
            button->internal.d_time = 0;
            gpio_config_t gpio;
            gpio.mode = GPIO_MODE_INPUT;
            gpio.pull_down_en = 0;
            gpio.pull_down_en = 0;
            gpio.intr_type = GPIO_INTR_DISABLE;
            gpio.pin_bit_mask = IO_MASK(button->gpio);
            gpio_config(&gpio);
            buttons[i] = button;
            xTaskResumeAll();
            break;
        }
    }
}
void button_remove_btn(button_t* button) {
    for (size_t i = 0; i < BTN_MAX; i++)
        if (buttons[i] == button) {
            vTaskSuspendAll();
            buttons[i] = NULL;
            xTaskResumeAll();
            break;
        }
}