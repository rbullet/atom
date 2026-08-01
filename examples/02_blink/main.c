#include <stdio.h>
#include <atom.h>

#define GPIO_LED_PIN 25

int main(void)
{
    gpio_config(GPIO_LED_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);

    while (1)
    {
        gpio_toggle(GPIO_LED_PIN);
        thread_sleep(duration_of(1, SECONDS));
    }

    return 0;
}