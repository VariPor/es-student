#include <stdio.h>
#include <string.h>

#include "logging/log.h"
#include "profiling/profiling.h"
#include "api/api.h"
#include "stdio-text-protocol/stdio-text-protocol.h"
#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "firmware.h"
#include "platform/platform.h"
#include "telemetry-task/telemetry-task.h"
#include "block-task/block-task.h"
#include "stream-task/stream-task.h"

enum stopwatch_numbers
{
    STOPWATCH_LOOP,
    STOPWATCH_COMMAND,
    STOPWATCH_LED,
    STOPWATCH_BUTTON,
    STOPWATCH_PI,
    STOPWATCH_TELEMETRY,
    STOPWATCH_DMA,
    STOPWATCH_STREAM
};

int main()
{
    LOG_INF("%s %s\n", FIRMWARE_NAME, FIRMWARE_VERSION);

    stdio_text_protocol_init();

    led_task_init();
    button_task_init(led_task_next_state);
    platform_init();
    telemetry_task_init(api_print_telemetry);
    stream_task_init(api_send_frame);

    // инициализация секундомеров
    profiling_stopwatch_init(STOPWATCH_LOOP, "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED, "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON, "button");
    profiling_stopwatch_init(STOPWATCH_PI, "pi");
    profiling_stopwatch_init(STOPWATCH_TELEMETRY, "telemetry");
    profiling_stopwatch_init(STOPWATCH_DMA, "dma");
    profiling_stopwatch_init(STOPWATCH_STREAM, "stream");
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

        profiling_start(STOPWATCH_TELEMETRY);
        telemetry_task_handle();
        profiling_stop(STOPWATCH_TELEMETRY);

        profiling_start(STOPWATCH_DMA);
        block_task_handle();
        profiling_stop(STOPWATCH_DMA);

        profiling_start(STOPWATCH_STREAM);
        stream_task_handle();
        profiling_stop(STOPWATCH_STREAM);

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