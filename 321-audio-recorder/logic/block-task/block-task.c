#include "block-task/block-task.h"
#include "platform/platform.h"
#include <stddef.h>

static uint32_t samples_count;
static block_task_callback_t on_block_callback = NULL;

// block_task_start(): захват уже идёт или platform его не начал — false;
// запомнить число отсчётов и функцию
bool block_task_start(uint32_t count, block_task_callback_t on_block)
{
    if (!platform_block_start(count))
    {
        return false;
    }
    samples_count = count;
    on_block_callback = on_block;
    return true;
}

// block_task_handle(): функции нет или блок не готов — выйти;
// забыть функцию, затем вызвать её с отсчётами, числом и временем
void block_task_handle(void)
{
    if (!on_block_callback || !platform_block_done())
    {
        return;
    }

    block_task_callback_t callback = on_block_callback;
    on_block_callback = NULL;

    callback(platform_block_samples(), samples_count, platform_block_time_us());
}
