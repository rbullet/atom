# Threads Example

This example demonstrates how to create and run multiple threads using ATOM.

The application starts two worker threads: one periodically prints a message to the console, while the other blinks the onboard LED. Meanwhile, the main thread continues to execute independently, illustrating concurrent execution.

## Demonstrates

* Thread initialization
* Thread startup
* Independent thread stacks
* Passing arguments to thread routines
* Thread sleeping
* Concurrent execution of multiple threads

## Source

The application creates two threads and starts them. Each thread performs a different task while the main thread continues to run:

```c
#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define GPIO_LED_PIN 25

#define HELLO_THREAD_STACK_SIZE 1024
#define BLINK_THREAD_STACK_SIZE 512

static uint32_t hello_thread_stack[HELLO_THREAD_STACK_SIZE];
static uint32_t blink_thread_stack[BLINK_THREAD_STACK_SIZE];

static void* logger_thread_routine(void* arg)
{
    (void)arg;

    while (1)
    {
        printf("Hello from thread!\r\n");
        thread_sleep(duration_of(1, SECONDS));
    }

    return NULL;
}

static void* blink_thread_routine(void* arg)
{
    uint32_t const pin = (uint32_t)(uintptr_t)arg;

    while (1)
    {
        gpio_toggle(pin);
        thread_sleep(duration_of(500, MILLISECONDS));
    }

    return NULL;
}

int main(void)
{
    gpio_config(GPIO_LED_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);

    thread_t hello_thread;

    thread_init(
        &hello_thread,
        hello_thread_stack,
        HELLO_THREAD_STACK_SIZE,
        logger_thread_routine,
        NULL
    );
    thread_start(&hello_thread);

    thread_t blink_thread;

    thread_init(
        &blink_thread,
        blink_thread_stack,
        BLINK_THREAD_STACK_SIZE,
        blink_thread_routine,
        (void *)(uintptr_t)GPIO_LED_PIN
    );
    thread_start(&blink_thread);

    while (1)
    {
        printf("Hello from main!\r\n");
        thread_sleep(duration_of(2, SECONDS));
    }

    return 0;
}
```

## Expected behavior

After flashing the example and opening the serial console, messages from the main thread and the logger thread are printed concurrently while the onboard LED blinks every 500 ms.

Example output:

```text
Hello from main!
Hello from thread!
Hello from thread!
Hello from main!
Hello from thread!
...
```

At the same time, the onboard LED continuously toggles on and off.

## Purpose

This example introduces ATOM's threading model.

Each thread has its own stack and executes independently under the control of the scheduler. Threads can voluntarily suspend execution using `thread_sleep()`, allowing other threads to run.

This example focuses on thread creation and concurrent execution. Synchronization primitives such as mutexes, semaphores, and condition variables are introduced in later examples.
