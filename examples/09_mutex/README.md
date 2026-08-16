# Mutex Example

This example demonstrates how to protect shared data between multiple threads using an ATOM mutex.

Two worker threads increment the same counter concurrently. The mutex ensures that each increment operation is performed safely, preventing race conditions.

## Demonstrates

* Creating multiple threads
* Protecting shared data with a mutex
* Using scoped mutex locking with `WITH_MUTEX`
* Waiting for thread completion with `thread_join()`
* Verifying synchronized access to shared state

## Source

The application starts two worker threads. Each thread increments the same shared counter while holding the mutex:

```c
#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define INCREMENT_THREAD_STACK_SIZE 1024

static uint32_t increment_thread_1_stack[INCREMENT_THREAD_STACK_SIZE];
static uint32_t increment_thread_2_stack[INCREMENT_THREAD_STACK_SIZE];

#define ITERATION_COUNT 500

static uint32_t shared_counter = 0;

static mutex_t mutex = MUTEX_INITIALIZER;

static void* increment_routine(void* arg)
{
    (void)arg;

    for (int i = 0; i < ITERATION_COUNT; i++)
    {
        WITH_MUTEX(&mutex)
        {
            shared_counter++;
        }
        WITH_MUTEX_END

        thread_sleep(duration_of(5, MILLISECONDS));
    }

    return NULL;
}

int main(void)
{
    thread_t increment_thread_1;

    thread_init(
        &increment_thread_1,
        increment_thread_1_stack,
        INCREMENT_THREAD_STACK_SIZE,
        increment_routine,
        NULL
    );
    thread_start(&increment_thread_1);

    thread_t increment_thread_2;

    thread_init(
        &increment_thread_2,
        increment_thread_2_stack,
        INCREMENT_THREAD_STACK_SIZE,
        increment_routine,
        NULL
    );
    thread_start(&increment_thread_2);

    printf("Waiting for workers...\r\n");

    thread_join(&increment_thread_1);
    thread_join(&increment_thread_2);

    printf(
        "Final value of shared_counter: %lu (expected %lu)\r\n",
        shared_counter,
        2 * ITERATION_COUNT
    );

    while (1)
    {
    }

    return 0;
}
```

## Expected behavior

After both worker threads complete, the application prints:

```text
Waiting for workers...
Final value of shared_counter: 1000 (expected 1000)
```

## Purpose

This example demonstrates why synchronization primitives are required when multiple threads access shared data.

The increment operation:

```c
shared_counter++;
```

is not atomic. Without a mutex, both threads could read the same value, increment it, and write the result back, causing lost updates.

The mutex guarantees that only one thread can execute the critical section at a time.

## Notes

The `WITH_MUTEX` macro automatically manages locking and unlocking around the protected section:

```c
WITH_MUTEX(&mutex)
{
    shared_counter++;
}
WITH_MUTEX_END
```

This keeps the critical section clearly scoped and prevents accidentally forgetting to release the mutex.
