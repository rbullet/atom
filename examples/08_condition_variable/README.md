# Condition Variable Example

This example demonstrates how to synchronize multiple threads using an ATOM condition variable.

Three student threads wait until the school day is over. Once the main thread announces the end of school, all waiting threads are notified and continue executing.

## Demonstrates

* Creating multiple threads
* Waiting for a shared condition
* Protecting shared state with a mutex
* Using `condition_variable_wait()`
* Waking all waiting threads with `condition_variable_broadcast()`

## Source

The application creates three student threads. Each thread waits until the `school_is_over` condition becomes true.

The main thread simulates the passing of time, updates the shared condition, and notifies all waiting threads.

```c
#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define STUDENT_THREAD_STACK_SIZE 1024

static uint32_t student1_thread_stack[STUDENT_THREAD_STACK_SIZE];
static uint32_t student2_thread_stack[STUDENT_THREAD_STACK_SIZE];
static uint32_t student3_thread_stack[STUDENT_THREAD_STACK_SIZE];

static bool school_is_over = false;

static mutex_t mutex = MUTEX_INITIALIZER;
static condition_variable_t end_of_school = CONDITION_VARIABLE_INITIALIZER;

static void* student_routine(void* arg)
{
    char const* const student_name = (char const* const)arg;

    WITH_MUTEX(&mutex)
    {
        while (!school_is_over)
        {
            condition_variable_wait(&end_of_school, &mutex);
        }

        printf("%s: It's the end of school! I'm going back home!\r\n", student_name);
    }

    return NULL;
}

int main(void)
{
    thread_t student1_thread;

    thread_init(
        &student1_thread,
        student1_thread_stack,
        STUDENT_THREAD_STACK_SIZE,
        student_routine,
        (void *)"Cartman"
    );
    thread_start(&student1_thread);

    thread_t student2_thread;

    thread_init(
        &student2_thread,
        student2_thread_stack,
        STUDENT_THREAD_STACK_SIZE,
        student_routine,
        (void *)"Kyle"
    );
    thread_start(&student2_thread);

    thread_t student3_thread;

    thread_init(
        &student3_thread,
        student3_thread_stack,
        STUDENT_THREAD_STACK_SIZE,
        student_routine,
        (void *)"Stan"
    );
    thread_start(&student3_thread);

    thread_sleep(duration_of(5, SECONDS));

    printf("Garrison: School is over! All students can go back home!\r\n");

    WITH_MUTEX(&mutex)
    {
        school_is_over = true;
        condition_variable_broadcast(&end_of_school);
    }

    while (1)
    {
    }

    return 0;
}
```

## Expected behavior

After starting the application, the student threads immediately begin waiting for the end of the school day.

Five seconds later, the main thread announces that school is over and wakes all waiting threads.

Example output:

```text
Garrison: School is over! All students can go back home!
Stan: It's the end of school! I'm going back home!
Kyle: It's the end of school! I'm going back home!
Cartman: It's the end of school! I'm going back home!
```

The order in which the students resume execution is not guaranteed and may vary between runs.

## Purpose

Condition variables allow threads to wait efficiently until a shared condition becomes true.

In this example, the shared condition is represented by the `school_is_over` flag. Each student thread checks this condition while holding the mutex:

```c
while (!school_is_over)
{
    condition_variable_wait(&end_of_school, &mutex);
}
```

When the school day ends, the main thread updates the shared state while holding the same mutex and broadcasts the condition variable:

```c
WITH_MUTEX(&mutex)
{
    school_is_over = true;
    condition_variable_broadcast(&end_of_school);
}
```

All waiting threads wake up, reacquire the mutex one at a time, verify that the condition is now true, and continue executing.

## Notes

Unlike a semaphore, a condition variable does not represent an event or maintain a count. Instead, it is used together with a shared condition protected by a mutex.

For this reason, a thread should always wait inside a loop that rechecks the condition after waking up.
