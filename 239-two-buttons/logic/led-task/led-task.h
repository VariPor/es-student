// задача светодиода: выключен, горит или мигает
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINK
} led_state_t;

typedef enum
{
    LED_PERIOD_250 = 250,
    LED_PERIOD_500 = 500,
    LED_PERIOD_1000 = 1000
} led_period_t;

void led_task_init(void);
void led_task_handle(void);

void led_task_set_state(led_state_t state);
led_state_t led_task_get_state(void);

// false - период не принят
bool led_task_set_period_ms(uint32_t period_ms);
uint32_t led_task_get_period_ms(void);

void led_task_next_state(void);
void led_task_next_period(void);

void led_task_blink(void);
