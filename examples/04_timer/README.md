# Deferred Task Example

This example demonstrates how to execute periodic work using ATOM's deferred task system.

Instead of running the LED toggle directly from the main loop, the application schedules a callback that is automatically invoked at a fixed interval by ATOM.

## Demonstrates

* Deferred task creation
* Periodic task execution
* Callback-based programming
* Passing user data to callbacks
* GPIO control from scheduled work

## Source

The application creates a periodic deferred task that toggles the onboard LED every second:

```c
#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define GPIO_LED_PIN 25

void blink_led(void* arg)
{
    uint32_t const pin = (uint32_t)(uintptr_t)arg;
    gpio_toggle(pin);
}

int main(void)
{
    gpio_config(GPIO_LED_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);

    deferred_task_t deferred_task;

    deferred_task_start_periodic(
        &deferred_task,
        duration_of(0, SECONDS),  // Initial delay
        duration_of(1, SECONDS),  // Period between each invocation
        blink_led,                // Callback
        (void*) GPIO_LED_PIN
    );

    while (1)
    {
    }

    return 0;
}
```

## Expected behavior

After flashing the example, the onboard LED will toggle once every second:

```text
LED ON
(wait 1 second)
LED OFF
(wait 1 second)
...
```

## Purpose

This example demonstrates a higher-level alternative to manually managing timing loops.

The application schedules work and lets ATOM handle when the callback should be executed. This allows the main application flow to remain available for other tasks.

Deferred tasks are useful for:

* periodic maintenance work
* delayed operations
* moving non-critical processing out of interrupt context
* scheduling background activities

## Notes

The callback receives a user-provided argument through `void*`. Since the GPIO pin number is an integer value rather than a pointer, it is converted through `uintptr_t` to safely pass it as callback data.
