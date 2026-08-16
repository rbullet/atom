#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define INCREMENT_THREAD_STACK_SIZE 1024
static uint32_t increment_thread_1_stack[INCREMENT_THREAD_STACK_SIZE];
static uint32_t increment_thread_2_stack[INCREMENT_THREAD_STACK_SIZE];

#define ITERATION_COUNT 500
static uint32_t shared_counter = 0;

static mutex_t mutex = MUTEX_INITIALIZER;

static void* increment_routine(void* const arg)
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
  thread_join(&increment_thread_1, NULL);
  thread_join(&increment_thread_2, NULL);

  printf("Final value of shared_counter: %lu (expected %lu)\r\n", shared_counter, 2 * ITERATION_COUNT);

  while (1)
  {
  }

  return 0;
}
