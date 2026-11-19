#include "edge-task/edge-task.h"
#include "systime/systime.h"
#include "logging/log.h"

typedef enum
{
    EDGE_TASK_STATE_IDLE,
    EDGE_TASK_STATE_DISCHARGE,
    EDGE_TASK_STATE_RISE,
    EDGE_TASK_STATE_CHARGE,
    EDGE_TASK_STATE_FALL
} edge_task_state_t;

static edge_task_state_t state;
static platform_line_mode_t line_mode;
static edge_task_callback_t callback;
static uint32_t phase_start;
static uint32_t time_edge_up;

// пороги
#define EMPTY_V 0.05f
#define RISE_V 2.0f
#define FULL_V 3.2f
#define FALL_V 1.3f
#define TIMEOUT_US 2000000

void enter(edge_task_state_t new_state, bool high)
{
    platform_line_set(line_mode, high);
    phase_start = systime_us();
    state = new_state;
}

void finish(void)
{
    state = EDGE_TASK_STATE_IDLE;
    platform_line_set(line_mode, false);
    callback(line_mode, time_edge_up < TIMEOUT_US, time_edge_up, systime_us() - phase_start);
}

bool edge_task_start(platform_line_mode_t mode, edge_task_callback_t on_done)
{
    if (state != EDGE_TASK_STATE_IDLE)
    {
        return false;
    }
    line_mode = mode;
    callback = on_done;
    enter(EDGE_TASK_STATE_DISCHARGE, false);
    return true;
}

void edge_task_handle(void) {
    switch (state)
    {
    case EDGE_TASK_STATE_IDLE:
        return;
    case EDGE_TASK_STATE_DISCHARGE:
        if (platform_voltage_read() < EMPTY_V) {
            enter(EDGE_TASK_STATE_RISE, true);
        }
        break;
    case EDGE_TASK_STATE_RISE:
        if (platform_voltage_read() > RISE_V) {
            time_edge_up = systime_us() - phase_start;
            enter(EDGE_TASK_STATE_CHARGE, true);
        }
        break;
    case EDGE_TASK_STATE_CHARGE:
        if (platform_voltage_read() > FULL_V) {
            enter(EDGE_TASK_STATE_FALL, false);
        }
        break;
        case EDGE_TASK_STATE_FALL:
        if (platform_voltage_read() < FALL_V) {
            finish();
        }
        break;
    }
    if (systime_us() - phase_start > TIMEOUT_US) {
        finish();
    }
}