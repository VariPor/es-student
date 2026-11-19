#include "platform.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

static const uint LED_PIN = 25;
static const uint BUTTON_PIN = 15;

// вход АЦП: вывод GP26, канал 0
#define ADC_PIN 26
#define ADC_CHANNEL 0

// опорное напряжение АЦП на плате Pico и число уровней 12-разрядного кода
#define ADC_VREF_V 3.3f
#define ADC_LEVELS 4096

// вывод, который через резистор 1 кОм ведёт линию RC-цепи
#define LINE_PIN 22

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void)
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    adc_init();
    adc_gpio_init(ADC_PIN);

    gpio_init(LINE_PIN);
    platform_line_set(PLATFORM_LINE_PUSH_PULL, false);
}

void platform_led_set(bool on)
{
    gpio_put(PICO_DEFAULT_LED_PIN, on);
}

bool platform_led_get(void)
{
    return gpio_get(PICO_DEFAULT_LED_PIN);
}

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(void)
{
    return !gpio_get(BUTTON_PIN);
}

float platform_voltage_read(void)
{
    adc_select_input(ADC_CHANNEL);
    return adc_read() * ADC_VREF_V / ADC_LEVELS;
}

void platform_line_set(platform_line_mode_t mode, bool high)
{
    if (mode == PLATFORM_LINE_PUSH_PULL)
    {
        // драйвер включён и сам задаёт уровень
        gpio_disable_pulls(LINE_PIN);
        gpio_put(LINE_PIN, high);
        gpio_set_dir(LINE_PIN, GPIO_OUT);
        return;
    }

    // открытый сток: в регистре выхода ноль, переключается только режим
    gpio_pull_up(LINE_PIN);
    gpio_put(LINE_PIN, 0);
    gpio_set_dir(LINE_PIN, high ? GPIO_IN : GPIO_OUT);
}
