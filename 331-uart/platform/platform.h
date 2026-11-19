#pragma once

#include <stdbool.h>

// режим вывода GP22, который ведёт на линию RC-цепи
typedef enum
{
    PLATFORM_LINE_PUSH_PULL,
    PLATFORM_LINE_OPEN_DRAIN
} platform_line_mode_t;

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

void platform_led_set(bool on);
bool platform_led_get(void);

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(void);

// напряжение на входе АЦП, выводе GP26б в вольтах
float platform_voltage_read(void);

// единица или ноль на линии; в режиме открытого стока единица -
// отпущенная линия, которую поднимает внутренняя подтяжка
void platform_line_set(platform_line_mode_t mode, bool high);
