#pragma once

#include <stdbool.h>
#include "pico/stdlib.h"


typedef enum
{
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_IS_PRESSING,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_IS_RELEASING
} button_state_t;

void button_task_init(void);
void button_task_handle(void);

bool button_task_is_pressed(void);
uint32_t button_task_get_press_count(void);