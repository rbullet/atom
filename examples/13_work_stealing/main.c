#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define THREAD_STACK_SIZE 1024

static uint32_t annie_thread_stack[THREAD_STACK_SIZE];
static uint32_t james_thread_stack[THREAD_STACK_SIZE];
static uint32_t brian_thread_stack[THREAD_STACK_SIZE];
static uint32_t sarah_thread_stack[THREAD_STACK_SIZE];

static mutex_t mutex = MUTEX_INITIALIZER;

static void* hello_thread_routine(void* arg)
{
  char* firstname = arg;
  while (1)
  {
    WITH_MUTEX(&mutex)
    {
      printf("%s: Hello from CORE %lu!\r\n", firstname, cpu_get_id());
    }
    WITH_MUTEX_END
  }
  return NULL;
}

int main(void)
{
  thread_t annie_thread;

  thread_init(
    &annie_thread,
    annie_thread_stack,
    THREAD_STACK_SIZE,
    hello_thread_routine,
    "Annie"
  );
  thread_start(&annie_thread);

  thread_t brian_thread;

  thread_init(
    &brian_thread,
    brian_thread_stack,
    THREAD_STACK_SIZE,
    hello_thread_routine,
    "Brian"
  );
  thread_start(&brian_thread);

  thread_t james_thread;

  thread_init(
    &james_thread,
    james_thread_stack,
    THREAD_STACK_SIZE,
    hello_thread_routine,
    "James"
  );
  thread_start(&james_thread);

  thread_t sarah_thread;

  thread_init(
    &sarah_thread,
    sarah_thread_stack,
    THREAD_STACK_SIZE,
    hello_thread_routine,
    "Sarah"
  );
  thread_start(&sarah_thread);

  thread_sleep(duration_of(5, SECONDS));

  printf("Starting the secondary core...\r\n");
  scheduler_start_secondary();
  while (1)
  {
  }
  return 0;
}
