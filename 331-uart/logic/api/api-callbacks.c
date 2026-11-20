#include "api/api-commands.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "device/device.h"
#include "profiling/profiling.h"
#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "firmware.h"
#include "systime/systime.h"
#include "platform/platform.h"
#include "edge-task/edge-task.h"
#include "uart-task/uart-task.h"

#define UART_CHARS_MAX 32

static void cmd_info(const command_t *command)
{
    // печатаем паспорт устройства
    struct device_info_t info;
    device_get_info(&info);
    printf("name: %s\n", FIRMWARE_NAME);
    printf("version: %s\n", FIRMWARE_VERSION);
    printf("project: %s\n", FIRMWARE_PROJECT);
    printf("repo: %s\n", FIRMWARE_REPO);
    printf("board: %s\n", info.device_board);
    printf("serial: %s\n", info.serial_number);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", info.manufacturer, info.part, info.revision);
    printf("pico-sdk: %s\n", info.pico_sdk_version);
}

static void cmd_uptime(const command_t *command)
{
    printf("uptime: %llu ms\n", systime_ms());
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

static void print_led_state(void)
{
    char *led_state_string[] = {"off", "on", "blink"};
    printf("led: %s, period %u ms\n",
           led_state_string[led_task_get_state()], (unsigned int)led_task_get_period_ms());
}

static void cmd_led_enable(const command_t *command)
{
    led_task_set_state(LED_STATE_ON);
    print_led_state();
}

static void cmd_led_disable(const command_t *command)
{
    led_task_set_state(LED_STATE_OFF);
    print_led_state();
}

static void cmd_led_blink(const command_t *command)
{
    led_task_set_state(LED_STATE_BLINK);
    print_led_state();
}

static void cmd_led_period(const command_t *command)
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

static void cmd_button(const command_t *command)
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
static const uint64_t CALC_PI_TERMS = 1000000;

static void cmd_pi_start(const command_t *command)
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

static void cmd_pi(const command_t *command)
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

static void cmd_profiling(const command_t *command)
{
#if PROFILING_ENABLED
    for (unsigned int i = 0; i < STOPWATCHES_COUNT; ++i)
    {
        struct profiling_result_t result;
        profiling_get(i, &result);
        printf("%-10s count %u mean %.2f us max %u us\n",
               result.name, (unsigned int)result.count,
               result.mean, (unsigned int)result.max_us);
    }
#else
    printf("profiling is disabled in this build\n");
#endif
}

static void cmd_profiling_reset(const command_t *command)
{
    profiling_reset();
    printf("profiling reset\n");
}

static void print_edge(platform_line_mode_t mode, bool ok,
                       uint32_t rise_us, uint32_t fall_us)
{
    if (!ok)
    {
        printf("edge %s: timeout\n", mode == PLATFORM_LINE_PUSH_PULL ? "pp" : "od");
        return;
    }
    printf("edge %s: rise %.1f ms, fall %.1f ms\n",
           mode == PLATFORM_LINE_PUSH_PULL ? "pp" : "od", rise_us / 1000.0, fall_us / 1000.0);
}

static void cmd_edge(const command_t *command)
{
    if (command->argc != 1 || (strcmp(command->argv[0], "pp") != 0 && strcmp(command->argv[0], "od") != 0))
    {
        printf("error: usage edge pp|od\n");
    }
    platform_line_mode_t mode = strcmp(command->argv[0], "pp") == 0 ? PLATFORM_LINE_PUSH_PULL : PLATFORM_LINE_OPEN_DRAIN;
    if (!edge_task_start(mode, print_edge))
    {
        printf("error: edge is busy\n");
    }
}

void print_bytes(const uint8_t *data, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (isprint(data[i]) == 0)
        {
            printf(".");
        }
        else
        {
            printf("%c", data[i]);
        }
    }
}

void print_errors(uint32_t errors)
{
    if (errors == 0)
    {
        printf(" err: none\n");
        return;
    }
    printf(" err: %lu %lu %lu %lu\n", errors & PLATFORM_UART_FE,
           errors & PLATFORM_UART_PE,
           errors & PLATFORM_UART_BE,
           errors & PLATFORM_UART_OE);
}

void cmd_loop(const command_t *command)
{
    const char *mode = command->argv[0];
    if (command->argc != 1 || (strcmp(mode, "internal") != 0 && strcmp(mode, "pins") != 0))
    {
        printf("error: usage loop internal|pins\n");
        return;
    }
    platform_uart_loop_set(strcmp(mode, "internal") == 0);
    printf("loop: %s\n", mode);
}

void print_echo(const uint8_t *data, size_t size, uint32_t errors)
{
    printf("rx: ");
    print_bytes(data, size);
    print_errors(errors);
}

void cmd_echo(const command_t *command)
{
    if (command->argc != 1)
    {
        printf("error: usage echo <text>\n");
        return;
    }

    if (strlen(command->argv[0]) > UART_CHARS_MAX)
    {
        printf("error: text longer than 32\n");
        return;
    }

    if (!uart_task_start(PLATFORM_UART0, PLATFORM_UART0,
                         (uint8_t *)command->argv[0], strlen(command->argv[0]), print_echo))
    {
        printf("error: uart is busy\n");
    }
}

const api_command_t api_commands[] = {
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
    {"profiling_reset", "reset profiling", cmd_profiling_reset},
    {"edge", "rise and fall of the RC line: edge pp|od", cmd_edge},
    {"loop", "UART0 loopback: loop internal|pins", cmd_loop},
    {"echo", "text through UART0 and back: echo <text>", cmd_echo}};

const uint32_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);
