#include "pico/stdlib.h"

// константы
const uint LED_PIN = 25;
const char DEVICE_NAME[] = "pico-blink";
const uint32_t PERIODS_MS[] = {250, 500, 1000};

// переменная с начальным значением
uint32_t blink_period_ms = 250;

// переменные с нулём и без значения
uint32_t blink_count = 0;
uint32_t last_time;
uint8_t log_buffer[64];

int main()
{
    // локальная переменная
    uint32_t delay = 250;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1)
    {
        gpio_put(LED_PIN, 1);
        sleep_ms(delay);
        gpio_put(LED_PIN, 0);
        sleep_ms(blink_period_ms);

        blink_count++;
        last_time = time_us_32();
        blink_period_ms = PERIODS_MS[blink_count % 3];
        log_buffer[blink_count % 64] = DEVICE_NAME[blink_count % 10];
    }
}