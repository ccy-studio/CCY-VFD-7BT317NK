#ifndef __COMPONENTS_BUTTON_H__
#define __COMPONENTS_BUTTON_H__

#include <stdint.h>
#include <stdbool.h>
#include <driver/gpio.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUTTON_MAX 5
#define BUTTON_POLL_TIMEOUT 10
#define BUTTON_LONG_PRESS_TIMEOUT 2500 //长按
#define BUTTON_AUTOREPEAT_TIMEOUT 500
#define BUTTON_AUTOREPEAT_INTERVAL 250

/**
 * Typedef of button descriptor
 */
typedef struct button_s button_t;

/**
 * Button states/events
 */
typedef enum {
    BUTTON_PRESSED = 0,
    BUTTON_RELEASED,
    BUTTON_CLICKED,
    BUTTON_PRESSED_LONG,
} button_state_t;

/**
 * Callback prototype
 *
 * @param btn    Pointer to button descriptor
 * @param state  Button action (new state)
 */
typedef void (*button_event_cb_t)(button_t *btn, button_state_t state);

/**
 * Button descriptor struct
 */
struct button_s
{
    gpio_num_t gpio;                //!< GPIO
    bool internal_pull;             //!< Enable internal pull-up/pull-down
    uint8_t pressed_level;          //!< Logic level of pressed button
    bool autorepeat;                //!< Enable autorepeat
    button_event_cb_t callback;     //!< Button callback
    void *ctx;                      //!< User data
    struct {
        button_state_t state;
        uint32_t pressed_time;
        uint32_t repeating_time;
    } internal;                     //!< Internal button state
};

/**
 * @brief Init button
 *
 * @param btn Pointer to button descriptor
 * @return `ESP_OK` on success
 */
esp_err_t button_init(button_t *btn);

/**
 * @brief Deinit button
 *
 * @param btn Pointer to button descriptor
 * @return `ESP_OK` on success
 */
esp_err_t button_done(button_t *btn);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* __COMPONENTS_BUTTON_H__ */