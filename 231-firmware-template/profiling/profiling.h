#pragma once

#include "pico/types.h"

enum
{
    STOPWATCHES_COUNT = 5
};

struct profiling_result_t
{
    const char *name;
    float mean;
    uint32_t max_us;
    uint32_t count;
};

void profiling_stopwatch_init(uint32_t id, const char *name);

void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);

// false - секундомер с таким номером не заведён
bool profiling_get(uint32_t id, struct profiling_result_t *result);
void profiling_reset(void);
