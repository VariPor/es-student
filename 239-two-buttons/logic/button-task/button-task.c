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

typedef struct
{
    int id;
    button_state_t button_state;
    uint32_t press_count;
    uint64_t last_time_toggle;
    button_task_callback_t button_callback;
} button_t;

// static button_state_t button_state;
// static uint32_t press_count = 0;
static const uint64_t DEBOUNCE_US = 20000;
// static uint64_t last_time_toggle = 0;
// static button_task_callback_t button_callback;

static button_t buttons[PLATFORM_BUTTON_COUNT];

void button_task_init(int id, button_task_callback_t on_press)
{
    buttons[id].button_state = BUTTON_STATE_RELEASED;
    buttons[id].press_count = 0;
    buttons[id].last_time_toggle = 0;
    buttons[id].button_callback = on_press;
}

void button_task_handle(void)
{
    // читаем часы
    uint64_t current_time = systime_us();
    for (int i = 0; i < PLATFORM_BUTTON_COUNT; ++i)
    {
        bool is_pressed = platform_button_read(i);
        switch (buttons[i].button_state)
        {
        // переходы по таблице; при переходе в «нажата»
        // увеличиваем счётчик и вызываем led_task_next_state()
        case BUTTON_STATE_PRESSED:
        {
            if (!is_pressed)
            {
                buttons[i].button_state = BUTTON_STATE_IS_RELEASING;
                buttons[i].last_time_toggle = current_time;
            }
            break;
        }
        case BUTTON_STATE_IS_PRESSING:
        {
            if (is_pressed && (current_time - buttons[i].last_time_toggle >= DEBOUNCE_US))
            {
                buttons[i].button_state = BUTTON_STATE_PRESSED;
                buttons[i].press_count += 1;
                if (buttons[i].button_callback != NULL)
                {
                    buttons[i].button_callback();
                }
                // last_time_toggle = current_time;
            }
            if (!is_pressed)
            {
                buttons[i].button_state = BUTTON_STATE_RELEASED;
                buttons[i].last_time_toggle = current_time;
            }
            break;
        }
        case BUTTON_STATE_IS_RELEASING:
        {
            if (!is_pressed && (current_time - buttons[i].last_time_toggle >= DEBOUNCE_US))
            {
                buttons[i].button_state = BUTTON_STATE_RELEASED;
                buttons[i].last_time_toggle = current_time;
            }
            if (is_pressed)
            {
                buttons[i].button_state = BUTTON_STATE_PRESSED;
                buttons[i].last_time_toggle = current_time;
            }
            break;
        }
        case BUTTON_STATE_RELEASED:
        {
            if (is_pressed)
            {
                buttons[i].button_state = BUTTON_STATE_IS_PRESSING;
                buttons[i].last_time_toggle = current_time;
            }
            break;
        }
        }
    }
}

bool button_task_is_pressed(int id)
{
    return buttons[id].button_state == BUTTON_STATE_PRESSED ||
           buttons[id].button_state == BUTTON_STATE_IS_RELEASING;
}

uint32_t button_task_get_press_count(int id)
{
    return buttons[id].press_count;
}
