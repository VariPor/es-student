#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "log.h"
#include "device.h"
#include "profiling.h"
#include "api.h"
#include "stdio-text-protocol.h"
#include "led-task.h"

int main()
{
    // инициализируем стандартный ввод-вывод и светодиод
    stdio_init_all();
    // запоминаем начальное показание часов для профилирования
    profiling_init();

    stdio_text_protocol_init();

    led_task_init();
    while (1)
    {
        // учитываем прошлую итерацию
        profiling_iteration();
        // мигаем светодиодом
        led_task_handle();
        // принятая строка становится командой, команда - действием прибора
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
    }
}