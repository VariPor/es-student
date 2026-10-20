#pragma once

#include <stdbool.h>

// номера кнопок на плате
typedef enum
{
    PLATFORM_BUTTON_LED,
    PLATFORM_BUTTON_PERIOD
} platform_button_t;

// количество кнопок на плате
enum
{
    PLATFORM_BUTTON_COUNT = 2
};

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

void platform_led_set(bool on);
bool platform_led_get(void);

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(int id);