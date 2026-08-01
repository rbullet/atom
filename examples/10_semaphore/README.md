# Semaphore Example

This example demonstrates how to use a semaphore to synchronize execution between multiple threads.

A producer thread periodically generates events, while a consumer thread waits for those events before processing them. The semaphore acts as a signaling mechanism between the two threads.

## Demonstrates

* Creating multiple threads
* Thread synchronization using semaphores
* Blocking a thread until an event occurs
* Producer/consumer communication pattern

## Source

The application creates two threads:

* The **producer thread** generates an event every two seconds and releases the semaphore.
* The **consumer thread** waits for the semaphore before processing the event.

```c id="k8q5xw"
#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define PRODUCER_THREAD_STACK_SIZE 1024
#define CONSUMER_THREAD_STACK_SIZE 1024

static uint32_t producer_thread_stack[PRODUCER_THREAD_STACK_SIZE];
static uint32_t consumer_thread_stack[CONSUMER_THREAD_STACK_SIZE];

static semaphore_t event_semaphore = SEMAPHORE_INITIALIZER(0);

static void* producer_routine(void* arg)
{
    (void)arg;

    while (1)
    {
        printf("Producer: event generated\r\n");

        semaphore_release(&event_semaphore);

        thread_sleep(duration_of(2, SECONDS));
    }

    return NULL;
}

static void* consumer_routine(void* arg)
{
    (void)arg;

    while (1)
    {
        semaphore_acquire(&event_semaphore);

        printf("Consumer: processing event\r\n");
    }

    return NULL;
}

int main(void)
{
    thread_t producer_thread;

    thread_init(
        &producer_thread,
        producer_thread_stack,
        PRODUCER_THREAD_STACK_SIZE,
        producer_routine,
        NULL
    );
    thread_start(&producer_thread);

    thread_t consumer_thread;

    thread_init(
        &consumer_thread,
        consumer_thread_stack,
        CONSUMER_THREAD_STACK_SIZE,
        consumer_routine,
        NULL
    );
    thread_start(&consumer_thread);

    while (1)
    {
    }

    return 0;
}
```

## Expected behavior

After starting the application, the console displays:

```text id="q7c7wp"
Producer: event generated
Consumer: processing event
Producer: event generated
Consumer: processing event
Producer: event generated
Consumer: processing event
```

The consumer thread remains blocked while waiting for the next event.

## Purpose

This example demonstrates how semaphores can be used to synchronize threads without requiring continuous polling.

The producer does not directly call the consumer. Instead, it signals that an event is available:

```c id="x9z0eu"
semaphore_release(&event_semaphore);
```

The consumer waits efficiently until the event occurs:

```c id="x2j6fg"
semaphore_acquire(&event_semaphore);
```

This pattern is commonly used in embedded systems where one component produces events and another component processes them later.

Examples include:

* A peripheral driver notifying a worker thread
* A DMA transfer completion signal
* A message queue consumer waiting for incoming data
* An interrupt handler waking a processing task

## Notes

A semaphore differs from a mutex:

* A **mutex** protects a shared resource and provides exclusive ownership.
* A **semaphore** provides signaling or counts available resources.

In this example, the semaphore is used as an event notification mechanism.
