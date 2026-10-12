#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "led.h"
#include "log.h"
#include "device.h"
#include "profiling.h"
#include "api.h"
#include "stdio-text-protocol.h"

const uint BLINK_HALF_PERIOD_MS = 500;
uint64_t last_toggle_us = 0;

void blink(void)
{
    uint64_t now_us = time_us_64();

    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000)
    {
        last_toggle_us = now_us;
        led_toggle();
    }
}

int main()
{
    // инициализируем стандартный ввод-вывод и светодиод
    stdio_init_all();
    led_init();
    // запоминаем начальное показание часов для профилирования
    profiling_init();

    stdio_text_protocol_init();
    while (1)
    {
        // учитываем прошлую итерацию
        profiling_iteration();
        // мигаем светодиодом
        blink();
        // принятая строка становится командой, команда - действием прибора
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
    }
}