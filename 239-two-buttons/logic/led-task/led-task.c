#include "led-task/led-task.h"

#include "systime/systime.h"
#include "platform/platform.h"

// Состояние задачи: внутреннее связывание, только для этого файла
static led_state_t led_state;
static uint64_t half_period_us;
static uint64_t last_toggle_us;

bool led_task_set_period_ms(uint32_t period_ms)
{
    // С нулевым полупериодом светодиод переключался бы
    // на каждой итерации: такой период не принимается
    if (period_ms == 0)
    {
        return false;
    }

    half_period_us = (uint64_t)period_ms * 1000 / 2;
    return true;
}

void led_task_init(void)
{
    // период 1000 мс, состояние «мигает»
    half_period_us = 1000 * 1000 / 2;
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    // запоминаем состояние и выполняем действие при входе:
    // «выключен» — гасим, «горит» — зажигаем, «мигает» — первое переключение сразу
    led_state = state;

    // Действие при входе: выполняется один раз, в момент перехода
    switch (led_state)
    {
    case LED_STATE_ON:
    {
        platform_led_set(true);
        break;
    }
    case LED_STATE_OFF:
    {
        platform_led_set(false);
        break;
    }
    case LED_STATE_BLINK:
    {
        // Первое переключение — на ближайшей итерации
        last_toggle_us = 0;
        break;
    }
    }
}

void led_task_handle(void)
{
    // «выключен» и «горит» — ничего не делаем, светодиод установлен при входе
    // «мигает» — каждые полпериода переключаем светодиод
    switch (led_state)
    {
    case LED_STATE_OFF:
    case LED_STATE_ON:
        // Светодиод установлен при входе в состояние, делать нечего
        break;
    case LED_STATE_BLINK:
        if (systime_period_elapsed(&last_toggle_us, half_period_us))
        {
            platform_led_set(!platform_led_get());
        }
        break;
    }
}

led_state_t led_task_get_state(void)
{
    return led_state;
}

uint32_t led_task_get_period_ms(void)
{
    return half_period_us / 1000 * 2;
}

void led_task_next_state(void)
{
    switch (led_state)
    {
    case LED_STATE_OFF:
        led_task_set_state(LED_STATE_ON);
        break;
    case LED_STATE_ON:
        led_task_set_state(LED_STATE_BLINK);
        break;
    case LED_STATE_BLINK:
        led_task_set_state(LED_STATE_OFF);
        break;
    }
}

void led_task_next_period(void)
{
    switch (half_period_us * 2 / 1000)
    {
    case LED_PERIOD_250:
        half_period_us = 250000;
        break;
    case LED_PERIOD_500:
        half_period_us = 500000;
        break;
    case LED_PERIOD_1000:
        half_period_us = 125000;
        break;
    default:
        half_period_us = 125000;
        break;
    }
}

void led_task_blink(void)
{
    led_task_set_state(LED_STATE_BLINK);
}
