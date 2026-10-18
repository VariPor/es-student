#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "logging/log.h"
#include "device/device.h"
#include "profiling/profiling.h"
#include "api.h"
#include "stdio-text-protocol/stdio-text-protocol.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"
#include "firmware.h"

enum stopwatch_numbers
{
    STOPWATCH_LOOP,
    STOPWATCH_COMMAND,
    STOPWATCH_LED,
    STOPWATCH_BUTTON,
    STOPWATCH_PI
};

int main()
{
    LOG_INF("%s %s\n", FIRMWARE_NAME, FIRMWARE_VERSION);

    // инициализируем стандартный ввод-вывод и светодиод
    stdio_init_all();

    stdio_text_protocol_init();

    led_task_init();
    button_task_init();

    // инициализация секундомеров
    profiling_stopwatch_init(STOPWATCH_LOOP, "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED, "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON, "button");
    profiling_stopwatch_init(STOPWATCH_PI, "pi");
    while (1)
    {
        profiling_start(STOPWATCH_LOOP);

        profiling_start(STOPWATCH_LED);
        led_task_handle();
        profiling_stop(STOPWATCH_LED);

        profiling_start(STOPWATCH_BUTTON);
        button_task_handle();
        profiling_stop(STOPWATCH_BUTTON);

        profiling_start(STOPWATCH_PI);
        pi_task_handle();
        profiling_stop(STOPWATCH_PI);

        profiling_start(STOPWATCH_COMMAND);
        // принятая строка становится командой, команда - действием прибора
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
        profiling_stop(STOPWATCH_COMMAND);

        profiling_stop(STOPWATCH_LOOP);
    }
}