/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-10-08 11:07:12
 * @LastEditTime: 2023-10-08 11:11:34
 */
#include "button.h"
#include <esp_timer.h>

#define DEAD_TIME_US 50000 // 50ms

#define POLL_TIMEOUT_US        (BUTTON_POLL_TIMEOUT * 1000)
#define AUTOREPEAT_TIMEOUT_US  (BUTTON_AUTOREPEAT_TIMEOUT * 1000)
#define AUTOREPEAT_INTERVAL_US (BUTTON_AUTOREPEAT_INTERVAL * 1000)
#define LONG_PRESS_TIMEOUT_US  (BUTTON_LONG_PRESS_TIMEOUT * 1000)

static button_t *buttons[BUTTON_MAX] = { NULL };
static esp_timer_handle_t timer = NULL;

#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)

static void poll_button(button_t *btn)
{
    if (btn->internal.state == BUTTON_PRESSED && btn->internal.pressed_time < DEAD_TIME_US)
    {
        // Dead time, ignore all
        btn->internal.pressed_time += POLL_TIMEOUT_US;
        return;
    }

    if (gpio_get_level(btn->gpio) == btn->pressed_level)
    {
        // button is pressed
        if (btn->internal.state == BUTTON_RELEASED)
        {
            // pressing just started, reset pressing/repeating time and run callback
            btn->internal.state = BUTTON_PRESSED;
            btn->internal.pressed_time = 0;
            btn->internal.repeating_time = 0;
            btn->callback(btn, BUTTON_PRESSED);
            return;
        }
        // increment pressing time
        btn->internal.pressed_time += POLL_TIMEOUT_US;

        // check autorepeat
        if (btn->autorepeat)
        {
            // check autorepeat timeout
            if (btn->internal.pressed_time < AUTOREPEAT_TIMEOUT_US)
                return;
            // increment repeating time
            btn->internal.repeating_time += POLL_TIMEOUT_US;

            if (btn->internal.repeating_time >= AUTOREPEAT_INTERVAL_US)
            {
                // reset repeating time and run callback
                btn->internal.repeating_time = 0;
                btn->callback(btn, BUTTON_CLICKED);
            }
            return;
        }

        if (btn->internal.state == BUTTON_PRESSED && btn->internal.pressed_time >= LONG_PRESS_TIMEOUT_US)
        {
            // button perssed long time, change state and run callback
            btn->internal.state = BUTTON_PRESSED_LONG;
            btn->callback(btn, BUTTON_PRESSED_LONG);
        }
    }
    else if (btn->internal.state != BUTTON_RELEASED)
    {
        // button released
        bool clicked = btn->internal.state == BUTTON_PRESSED;
        btn->internal.state = BUTTON_RELEASED;
        btn->callback(btn, BUTTON_RELEASED);
        if (clicked)
            btn->callback(btn, BUTTON_CLICKED);
    }
}

static void poll(void *arg)
{
    for (size_t i = 0; i < BUTTON_MAX; i++)
        if (buttons[i] && buttons[i]->callback)
            poll_button(buttons[i]);
}

////////////////////////////////////////////////////////////////////////////////

static const esp_timer_create_args_t timer_args = {
    .arg = NULL,
    .name = "poll_buttons",
    .dispatch_method = ESP_TIMER_TASK,
    .callback = poll,
};

esp_err_t button_init(button_t *btn)
{
    CHECK_ARG(btn);

    if (!timer)
        CHECK(esp_timer_create(&timer_args, &timer));

    esp_timer_stop(timer);

    esp_err_t res = ESP_ERR_NO_MEM;

    for (size_t i = 0; i < BUTTON_MAX; i++)
    {
        if (buttons[i] == btn)
            break;

        if (!buttons[i])
        {
            btn->internal.state = BUTTON_RELEASED;
            btn->internal.pressed_time = 0;
            btn->internal.repeating_time = 0;
            res = gpio_set_direction(btn->gpio, GPIO_MODE_INPUT);
            if (res != ESP_OK) break;
            if (btn->internal_pull)
            {
                res = gpio_set_pull_mode(btn->gpio, btn->pressed_level ? GPIO_PULLDOWN_ONLY : GPIO_PULLUP_ONLY);
                if (res != ESP_OK) break;
            }
            buttons[i] = btn;
            break;
        }
    }

    CHECK(esp_timer_start_periodic(timer, POLL_TIMEOUT_US));
    return res;
}

esp_err_t button_done(button_t *btn)
{
    CHECK_ARG(btn);

    esp_timer_stop(timer);

    esp_err_t res = ESP_ERR_INVALID_ARG;

    for (size_t i = 0; i < BUTTON_MAX; i++)
        if (buttons[i] == btn)
        {
            buttons[i] = NULL;
            res = ESP_OK;
            break;
        }

    CHECK(esp_timer_start_periodic(timer, POLL_TIMEOUT_US));
    return res;
}