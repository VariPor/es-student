#include "pi-task.h"
#include "pico/stdlib.h"
#include "systime/systime.h"

// расчёт одного члена ряда около 585 тактов
// при 125 МГц это около 4,7 мкс
// обработчик задачи должен уложиться в 0,5 мс, это около 100 членов
#define PI_TERMS_PER_CALL 100

// состояние расчёта: должно пережить возврат из обработчика
static pi_state_t pi_state = PI_STATE_NOT_START;
static uint32_t pi_k;
static uint32_t pi_n;
static double pi_sum;
static double pi_sign;
static double pi_result;
static uint64_t start_time = 0;
static uint64_t end_time = 0;

bool pi_task_start(uint32_t n)
{
    if (pi_state == PI_STATE_RUNNING)
    {
        return false;
    }
    pi_n = n;
    pi_k = 0;
    pi_sum = 0;
    pi_sign = 1.0;
    pi_state = PI_STATE_RUNNING;
    start_time = systime_us();
    return true;
}

void pi_task_handle(void)
{
    if (pi_state != PI_STATE_RUNNING)
    {
        return;
    }

    // очередная порция членов ряда, но не дальше последнего
    uint32_t end = pi_k + PI_TERMS_PER_CALL;
    if (end > pi_n || pi_k > pi_n)
    {
        end = pi_n;
    }
    for (; pi_k < end; pi_k++)
    {
        pi_sum += pi_sign / (2.0 * pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    if (pi_k == pi_n)
    {
        pi_result = 4.0 * pi_sum;
        pi_state = PI_STATE_READY;
        end_time = systime_us();
    }
}

bool pi_task_get_result(double *result, uint64_t *calc_time)
{
    if (pi_state != PI_STATE_READY)
    {
        return false;
    }
    *result = pi_result;
    *calc_time = end_time - start_time;
    return true;
}

pi_state_t pi_task_get_state(void)
{
    return pi_state;
}