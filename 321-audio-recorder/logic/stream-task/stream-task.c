#include "stream-task/stream-task.h"
#include "platform/platform.h"
#include "stddef.h"
#include "logging/log.h"

static stream_task_callback_t on_frame_callback;
static stream_state_t stream_state;
static uint8_t stream_bits;
static uint32_t stream_frame_num;
static uint32_t stream_samples;

// stream_task_init(): запомнить функцию для кадров, состояние IDLE
void stream_task_init(stream_task_callback_t on_frame)
{
    on_frame_callback = on_frame;
    stream_state = STREAM_STATE_IDLE;
}

// stream_task_handle(): в состоянии RUN, если platform отдал буфер:
//     заголовок — синхрослово STREAM_SYNC_WORD, номер, число отсчётов
//     байтов — отсчётов × 2 при 16 битах, отсчётов при 8
//     отдать заголовок и буфер функции, буфер вернуть platform
//     номер, число кадров и отсчётов — вперёд
void stream_task_handle(void)
{
    if (stream_state != STREAM_STATE_RUN)
    {
        return;
    }

    uint32_t current_stream_samples;
    const uint8_t *next_buf = platform_stream_next(&current_stream_samples);
    if (next_buf == NULL)
    {
        return;
    }

    stream_header_t stream_header = {STREAM_SYNC_WORD, stream_frame_num, current_stream_samples};
    uint32_t bytes = stream_bits * current_stream_samples / 8;
    on_frame_callback(&stream_header, next_buf, bytes);
    platform_stream_release();
    stream_frame_num += 1;
    stream_samples += current_stream_samples;
}

// stream_task_start(): поток уже идёт или platform его не начал — false;
// запомнить разрядность, номер кадра и итог — с нуля, состояние RUN
bool stream_task_start(uint8_t bits)
{
    if (stream_state == STREAM_STATE_RUN)
    {
        return false;
    }

    if (!platform_stream_start(bits))
    {
        return false;
    }
    stream_frame_num = 0;
    stream_samples = 0;
    stream_bits = bits;
    stream_state = STREAM_STATE_RUN;
    return true;
}

// stream_task_stop(): остановить поток в platform, состояние IDLE
void stream_task_stop(void)
{
    platform_stream_stop();
    stream_state = STREAM_STATE_IDLE;
}

stream_state_t stream_task_get_state(void)
{
    return stream_state;
}

uint8_t stream_task_get_bits(void)
{
    return stream_bits;
}

// итог потока с последнего запуска
uint32_t stream_task_get_frames(void)
{
    return stream_frame_num;
}

uint32_t stream_task_get_samples(void)
{
    return stream_samples;
}

uint32_t stream_task_get_overruns(void)
{
    return platform_stream_overruns();
}