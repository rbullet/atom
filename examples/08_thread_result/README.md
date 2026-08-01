# Thread Result Example

This example demonstrates how a thread can perform a task, return a result, and allow another thread to retrieve that result using `thread_join()`.

The example simulates a simple interaction where the main thread asks Alice to solve an equation. Alice performs the calculation in a separate thread, writes the result on a shared "blackboard", and returns the result when the task is complete.

## Demonstrates

* Creating a thread that performs a finite task
* Waiting for a thread to complete with `thread_join()`
* Returning a result from a thread
* Handling data lifetime after a thread exits

## Source

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <atom.h>

#define ALICE_THREAD_STACK_SIZE 1024

static uint32_t alice_thread_stack[ALICE_THREAD_STACK_SIZE];
static char blackboard[256];

static void* alice_thread_routine(void* arg)
{
  (void)arg;

  printf("Give me a second, I'll solve your equation.\r\n");

  thread_sleep(duration_of(5, SECONDS));

  strcpy(blackboard, "x = 5.");

  return blackboard;
}

int main(void)
{
  thread_t alice_thread;

  thread_init(
    &alice_thread,
    alice_thread_stack,
    ALICE_THREAD_STACK_SIZE,
    alice_thread_routine,
    NULL
  );

  printf("Hey Alice, if 2x = 10, what is the value of x?\r\n");

  thread_start(&alice_thread);

  char const* answer = thread_join(&alice_thread);

  printf("Alice answered: %s\r\n", answer);

  while (1)
  {
  }

  return 0;
}
```

## Expected output

```text
Hey Alice, if 2x = 10, what is the value of x?
Give me a second, I'll solve your equation.
Alice answered: x = 5.
```

## How it works

The main thread starts Alice's thread:

```c
thread_start(&alice_thread);
```

Alice performs the calculation independently:

```c
thread_sleep(duration_of(5, SECONDS));
```

The sleep simulates a longer computation.

After finishing, Alice stores the result:

```c
strcpy(blackboard, "x = 5.");
```

and returns it:

```c
return blackboard;
```

The main thread waits until Alice has completed:

```c
char const* answer = thread_join(&alice_thread);
```

Once the thread has finished, `thread_join()` returns the value provided by the thread routine.

## Why use a shared result buffer?

A thread's stack is no longer valid after the thread exits. Returning a pointer to a local variable would therefore be unsafe.

For example, this would be incorrect:

```c
static void* alice_thread_routine(void* arg)
{
  char answer[32];

  strcpy(answer, "x = 5.");

  return answer;
}
```

The `answer` buffer belongs to the thread stack and disappears when the function returns.

Instead, this example uses:

```c
static char blackboard[256];
```

The blackboard exists independently of Alice's thread, so the main thread can safely read the result after `thread_join()` returns.

## Purpose

Threads are not limited to running forever in the background. They can also represent tasks that:

* perform a computation
* process data
* communicate a result
* terminate when their work is complete

`thread_join()` provides a way for another thread to wait for completion and retrieve the result of that work.
