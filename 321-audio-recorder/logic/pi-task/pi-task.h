#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    PI_STATE_RUNNING,
    PI_STATE_READY,
    PI_STATE_NOT_START,
} pi_state_t;

bool pi_task_start(uint32_t n);
void pi_task_handle(void);
bool pi_task_get_result(double *result, uint64_t *calc_time);
pi_state_t pi_task_get_state(void);