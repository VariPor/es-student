#include "telemetry-task.h"

#include "logging/log.h"
#include "systime/systime.h"
#include "platform/platform.h"

static telemetry_task_callback_t telemetry_callback;
static telemetry_state_t telemetry_state;
static uint64_t telemetry_period_us;
static uint64_t telemetry_last_record_us;

// telemetry_task_init(): запомнить функцию для записей, состояние IDLE
void telemetry_task_init(telemetry_task_callback_t on_record)
{
    telemetry_callback = on_record;
    telemetry_state = TELEMETRY_STATE_IDLE;
}

// telemetry_task_start(): период меньше 10 мс не принять,
// запомнить период, первую запись — на ближайшей итерации, состояние RUN
bool telemetry_task_start(uint32_t period_ms)
{
    if (period_ms < 10)
    {
        return false;
    }
    telemetry_period_us = period_ms * 1000;
    telemetry_last_record_us = 0;
    telemetry_state = TELEMETRY_STATE_RUN;
    return true;
}

// telemetry_task_stop(): состояние IDLE
void telemetry_task_stop(void)
{
    telemetry_state = TELEMETRY_STATE_IDLE;
}

// telemetry_task_handle():
// в состоянии RUN, если прошёл период:
//     записать время systime_us(), напряжение и температуру у platform
//     отдать запись функции, полученной при запуске
void telemetry_task_handle(void)
{
    if (telemetry_state != TELEMETRY_STATE_RUN)
    {
        if (telemetry_state != TELEMETRY_STATE_IDLE)
        {
            LOG_ERR("No such telemetry state\n");
        }
        return;
    }
    if (systime_period_elapsed(&telemetry_last_record_us, telemetry_period_us))
    {
        telemetry_record_t record;
        record.time_us = systime_us();
        record.voltage_v = platform_voltage_read();
        record.temp_c = platform_temp_read();
        telemetry_callback(&record);
    }
}