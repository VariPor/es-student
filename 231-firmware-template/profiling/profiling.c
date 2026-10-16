#include "profiling.h"
#include "pico/stdlib.h"

// итерация около 2 мкс, за секунду их около 500 000
// ближайшая степень двойки 2^19 = 524288, среднее помнит около секунды
#define AVG_SHIFT 19
// замер в 1/256 долях микросекунды, чтобы среднее не теряло дробную часть
#define FRACTION_SHIFT 8

typedef struct
{
    const char *name;
    uint64_t avg_sum;
    uint32_t previous_us;
    uint32_t max_us;
    uint32_t count;
} stopwatch_t;

static stopwatch_t stopwatches[STOPWATCHES_COUNT];

void profiling_stopwatch_init(uint32_t id, const char *name)
{
    stopwatches[id].name = name;
    stopwatches[id].avg_sum = 0;
    stopwatches[id].previous_us = time_us_32();
    stopwatches[id].max_us = 0;
    stopwatches[id].count = 0;
}

void profiling_start(uint32_t id)
{
    stopwatches[id].previous_us = time_us_32();
}

void profiling_stop(uint32_t id)
{
    uint32_t now_us = time_us_32();
    uint32_t iteration_us = now_us - stopwatches[id].previous_us;
    stopwatches[id].count += 1;

    if (iteration_us > stopwatches[id].max_us)
    {
        stopwatches[id].max_us = iteration_us;
    }

    // обновление среднего
    stopwatches[id].avg_sum = stopwatches[id].avg_sum -
                              (stopwatches[id].avg_sum >> AVG_SHIFT) +
                              ((uint64_t)iteration_us << FRACTION_SHIFT);
}

bool profiling_get(uint32_t id, struct profiling_result_t *result)
{
    if (id >= STOPWATCHES_COUNT)
    {
        return false;
    }
    result->name = stopwatches[id].name;
    result->mean = (float)(stopwatches[id].avg_sum >> AVG_SHIFT) / (1 << FRACTION_SHIFT);
    result->max_us = stopwatches[id].max_us;
    result->count = stopwatches[id].count;
    return true;
}
void profiling_reset(void)
{
    for (unsigned int i = 0; i < STOPWATCHES_COUNT; ++i)
    {
        stopwatches[i].max_us = 0;
        stopwatches[i].avg_sum = 0;
        stopwatches[i].count = 0;
    }
}