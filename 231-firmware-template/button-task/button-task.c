#include "button-task.h"
#include "pico/stdlib.h"
#include "led-task.h"
#include "logging/log.h"
#include "systime/systime.h"

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
static const uint BUTTON_PIN = 15;

void button_task_init(void)
{
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    button_state = BUTTON_STATE_RELEASED;
}

void button_task_handle(void)
{
    // читаем уровень: нажатая кнопка замыкает вывод на землю и даёт ноль
    bool state = gpio_get(BUTTON_PIN);
    // читаем часы
    uint64_t current_time = systime_us();

    switch (button_state)
    {
    // переходы по таблице; при переходе в «нажата»
    // увеличиваем счётчик и вызываем led_task_next_state()
    case BUTTON_STATE_PRESSED:
    {
        if (state)
        {
            button_state = BUTTON_STATE_IS_RELEASING;
            last_time_toggle = current_time;
        }
        break;
    }
    case BUTTON_STATE_IS_PRESSING:
    {
        if (!state && (current_time - last_time_toggle >= DEBOUNCE_US))
        {
            button_state = BUTTON_STATE_PRESSED;
            press_count += 1;
            led_task_next_state();
            //last_time_toggle = current_time;
        }
        if (state)
        {
            button_state = BUTTON_STATE_RELEASED;
            last_time_toggle = current_time;
        }
        break;
    }
    case BUTTON_STATE_IS_RELEASING:
    {
        if (state && (current_time - last_time_toggle >= DEBOUNCE_US))
        {
            button_state = BUTTON_STATE_RELEASED;
            last_time_toggle = current_time;
        }
        if (!state)
        {
            button_state = BUTTON_STATE_PRESSED;
            last_time_toggle = current_time;
        }
        break;
    }
    case BUTTON_STATE_RELEASED:
    {
        if (!state)
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
