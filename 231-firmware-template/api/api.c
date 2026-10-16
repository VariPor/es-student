#include "api.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "device.h"
#include "pico/stdlib.h"
#include "profiling.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"

// обработчик команды: получает имя и аргументы
typedef void (*api_callback_t)(const command_t *command);

// команда прибора: имя, справка для help и обработчик
typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;

void cmd_info(const command_t *command)
{
    // печатаем паспорт устройства
    struct device_info_t info;
    device_get_info(&info);
    printf("project: %s\n", DEVICE_PROJECT);
    printf("repo: %s\n", DEVICE_REPO);
    printf("board: %s\n", info.device_board);
    printf("serial: %s\n", info.serial_number);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", info.manufacturer, info.part, info.revision);
    printf("pico-sdk: %s\n", PICO_SDK_VERSION_STRING);
}

void cmd_uptime(const command_t *command)
{
    printf("uptime: %u ms\n", (unsigned int)(time_us_64() / 1000));
}

// Число без знака из десятичной записи. false - в записи не только цифры
// или число не помещается в uint32_t
static bool parse_u32(const char *text, uint32_t *value)
{
    // strtoul приняла бы и знак: из "-1" получилось бы 4294967295
    if (!isdigit((unsigned char)text[0]))
    {
        return false;
    }

    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);

    // после числа остались символы или число не поместилось
    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return false;
    }

    *value = (uint32_t)number;
    return true;
}

void print_led_state(void)
{
    char *led_state_string[] = {"off", "on", "blink"};
    printf("led: %s, period %u ms\n",
           led_state_string[led_task_get_state()], (unsigned int)led_task_get_period_ms());
}

void cmd_led_enable(const command_t *command)
{
    led_task_set_state(LED_STATE_ON);
    print_led_state();
}

void cmd_led_disable(const command_t *command)
{
    led_task_set_state(LED_STATE_OFF);
    print_led_state();
}

void cmd_led_blink(const command_t *command)
{
    led_task_set_state(LED_STATE_BLINK);
    print_led_state();
}

void cmd_led_period(const command_t *command)
{
    uint32_t period_ms;

    if (command->argc != 1 || !parse_u32(command->argv[0], &period_ms))
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }

    // годится ли такой период, решает задача светодиода
    if (!led_task_set_period_ms(period_ms))
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }

    // ответ: состояние и период светодиода
    print_led_state();
}

void cmd_button(const command_t *command)
{
    char state[] = "released";
    if (button_task_is_pressed())
    {
        strcpy(state, "pressed");
    }
    printf("button: %s, presses %u\n", state, (unsigned int)button_task_get_press_count());
}

// прикидка: за член ряда 4 операции с double: 175 + 110 + 190 + 110 = 585
// 1e6 членов по 585 тактов при 125 МГц  - около 4,7 с
const uint CALC_PI_TERMS = 1000000;

void cmd_pi_start(const command_t *command)
{
    uint32_t pi_n = CALC_PI_TERMS;
    if (command->argc != 0 && (command->argc != 1 || !parse_u32(command->argv[0], &pi_n)))
    {
        printf("error: usage pi_start [<terms>]\n");
        return;
    }
    if (pi_n == 0)
    {
        printf("error: terms must be greater than 0\n");
        return;
    }
    if (!pi_task_start(pi_n))
    {
        printf("error: pi is busy\n");
    }
    printf("pi: started, %u terms\n", (unsigned int)pi_n);
}

void cmd_pi(const command_t *command)
{
    pi_state_t state = pi_task_get_state();
    switch (state)
    {
    case PI_STATE_NOT_START:
        printf("pi: not started\n");
        break;
    case PI_STATE_RUNNING:
        printf("pi: running\n");
        break;
    case PI_STATE_READY:
        double pi_result;
        uint64_t pi_time;
        if (pi_task_get_result(&pi_result, &pi_time))
        {
            printf("pi: %.9f in %llu ms\n", pi_result, pi_time / 1000);
        }
        break;
    }
}

void cmd_profiling(const command_t *command)
{
    for (unsigned int i = 0; i < STOPWATCHES_COUNT; ++i)
    {
        struct profiling_result_t result;
        profiling_get(i, &result);
        printf("%-10s count %u mean %.2f us max %u us\n",
               result.name, (unsigned int)result.count,
               result.mean, (unsigned int)result.max_us);
    }
}

void cmd_profiling_reset(const command_t *command)
{
    profiling_reset();
}

const api_command_t commands[] = {
    {"info", "device passport", cmd_info},
    {"uptime", "time since reset", cmd_uptime},
    {"led_enable", "enable led", cmd_led_enable},
    {"led_disable", "disable led", cmd_led_disable},
    {"led_blink", "set led blink", cmd_led_blink},
    {"led_period", "set led period: led_period [period_ms]", cmd_led_period},
    {"button", "button info", cmd_button},
    {"pi_start", "start calculating pi", cmd_pi_start},
    {"pi", "result calculating pi", cmd_pi},
    {"profiling", "print results of profiling", cmd_profiling},
    {"profiling_reset", "reset profiling", cmd_profiling_reset}};

static const uint api_command_count = sizeof(commands) / sizeof(commands[0]);

void api_handle(const command_t *command)
{
    if (command->truncated)
    {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }
    if (strcmp(command->name, "help") == 0)
    {
        printf("%-20s list of commands\n", "help");
        for (uint i = 0; i < api_command_count; ++i)
        {
            printf("%-20s %s\n", commands[i].name, commands[i].help);
        }
        return;
    }
    for (uint i = 0; i < api_command_count; ++i)
    {
        if (strcmp(commands[i].name, command->name) == 0)
        {
            commands[i].callback(command);
            return;
        }
    }
    printf("error: unknown command '%s', try help\n", command->name);
}
