#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*button_task_callback_t)(void);

void button_task_init(int id, button_task_callback_t on_press);
void button_task_handle(void);

bool button_task_is_pressed(int id);
uint32_t button_task_get_press_count(int id);