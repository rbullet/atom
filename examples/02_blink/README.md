# Blink Example

This example demonstrates basic GPIO control with ATOM by periodically toggling the onboard LED.

## Demonstrates

* ATOM initialization
* GPIO configuration
* Digital output control
* Thread sleep and timing utilities

## Source

The application configures the onboard LED GPIO as an output and toggles it every second:

```c
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
```

## Expected behavior

After flashing the example, the onboard LED will blink continuously:

```
LED ON
(wait 1 second)
LED OFF
(wait 1 second)
...
```

## Purpose

This example verifies that ATOM can interact with hardware peripherals and use its timing facilities from an application.

It is the first hardware-oriented example and serves as a simple validation of the GPIO API and scheduler timing functions.
