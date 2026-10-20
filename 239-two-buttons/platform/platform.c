#include "platform.h"
#include "pico/stdlib.h"

static const uint LED_PIN = 25;
static const uint BUTTON_LED_PIN = 15;
static const uint BUTTON_PERIOD_PIN = 14;

static void button_init(uint pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void)
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    button_init(BUTTON_LED_PIN);
    button_init(BUTTON_PERIOD_PIN);
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
bool platform_button_read(int id)
{
    if (id == PLATFORM_BUTTON_LED)
        return !gpio_get(BUTTON_LED_PIN);
    if (id == PLATFORM_BUTTON_PERIOD)
        return !gpio_get(BUTTON_PERIOD_PIN);
    return false;
}
