#pragma once

// по умолчанию профилирование включено
#ifndef PROFILING_ENABLED
#define PROFILING_ENABLED 1
#endif

#include <stdint.h>
#include <stdbool.h>

enum
{
    STOPWATCHES_COUNT = 6
};

struct profiling_result_t
{
    const char *name;
    uint64_t max_us;
    uint64_t count;
    float mean;
};

void profiling_stopwatch_init(uint32_t id, const char *name);


#if PROFILING_ENABLED
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
#else
// профилирование выключено: вызовы ничего не делают
#define profiling_start(id) ((void)0)
#define profiling_stop(id) ((void)0)
#endif

// false - секундомер с таким номером не заведён
bool profiling_get(uint32_t id, struct profiling_result_t *result);
void profiling_reset(void);
