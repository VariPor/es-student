#include "button-task/button-task.h"

#include "logging/log.h"
#include "systime/systime.h"
#include "platform/platform.h"

typedef enum
{
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_IS_PRESSING,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_IS_RELEASING
} button_state_t;

static button_state_t button_state;
static uint32_t press_count = 0;
static const uint64_t DEBOUNCE_US = 20000;
static uint64_t last_time_toggle = 0;
static button_task_callback_t button_callback;

void button_task_init(button_task_callback_t on_press)
{
    button_state = BUTTON_STATE_RELEASED;
    button_callback = on_press;
}

void button_task_handle(void)
{
    bool is_pressed = platform_button_read();
    // читаем часы
    uint64_t current_time = systime_us();

    switch (button_state)
    {
    // переходы по таблице; при переходе в «нажата»
    // увеличиваем счётчик и вызываем led_task_next_state()
    case BUTTON_STATE_PRESSED:
    {
        if (!is_pressed)
        {
            button_state = BUTTON_STATE_IS_RELEASING;
            last_time_toggle = current_time;
        }
        break;
    }
    case BUTTON_STATE_IS_PRESSING:
    {
        if (is_pressed && (current_time - last_time_toggle >= DEBOUNCE_US))
        {
            button_state = BUTTON_STATE_PRESSED;
            press_count += 1;
            if (button_callback != NULL)
            {
                button_callback();
            }
            // last_time_toggle = current_time;
        }
        if (!is_pressed)
        {
            button_state = BUTTON_STATE_RELEASED;
            last_time_toggle = current_time;
        }
        break;
    }
    case BUTTON_STATE_IS_RELEASING:
    {
        if (!is_pressed && (current_time - last_time_toggle >= DEBOUNCE_US))
        {
            button_state = BUTTON_STATE_RELEASED;
            last_time_toggle = current_time;
        }
        if (is_pressed)
        {
            button_state = BUTTON_STATE_PRESSED;
            last_time_toggle = current_time;
        }
        break;
    }
    case BUTTON_STATE_RELEASED:
    {
        if (is_pressed)
        {
            button_state = BUTTON_STATE_IS_PRESSING;
            last_time_toggle = current_time;
        }
        break;
    }
    }
}

bool button_task_is_pressed(void)
{
    return button_state == BUTTON_STATE_PRESSED || button_state == BUTTON_STATE_IS_RELEASING;
}

uint32_t button_task_get_press_count(void)
{
    return press_count;
}
