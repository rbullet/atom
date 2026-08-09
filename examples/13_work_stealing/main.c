#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#include "../../src/main/arch/rp2040/include/rp2040/concurrent/interrupts.h"
#include "../../src/main/arch/rp2040/include/rp2040/system/cpu.h"
#include "concurrent/scheduler.h"

#define THREAD_STACK_SIZE 1024

static uint32_t annie_thread_stack[THREAD_STACK_SIZE];
static uint32_t james_thread_stack[THREAD_STACK_SIZE];
static uint32_t brian_thread_stack[THREAD_STACK_SIZE];
static uint32_t sarah_thread_stack[THREAD_STACK_SIZE];

static mutex_t mutex = MUTEX_INITIALIZER;

static void* annie_thread_routine(void* arg)
{
  while (1)
  {
    WITH_MUTEX(&mutex)
    {
      printf("Annie: Hello from CORE %lu!\r\n", CPUID);
    }
  }
  return NULL;
}

static void* brian_thread_routine(void* arg)
{
  while (1)
  {
    WITH_MUTEX(&mutex)
    {
      printf("Brian: Hello from CORE %lu!\r\n", CPUID);
    }
  }
  return NULL;
}

static void* james_thread_routine(void* arg)
{
  while (1)
  {
    WITH_MUTEX(&mutex)
    {
      printf("James: Hello from CORE %lu!\r\n", CPUID);
    }
  }
  return NULL;
}

static void* sarah_thread_routine(void* arg)
{
  while (1)
  {
    WITH_MUTEX(&mutex)
    {
      printf("Sarah: Hello from CORE %lu!\r\n", CPUID);
    }
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
    annie_thread_routine,
    NULL
  );
  thread_start(&annie_thread);

  thread_t brian_thread;

  thread_init(
    &brian_thread,
    brian_thread_stack,
    THREAD_STACK_SIZE,
    brian_thread_routine,
    NULL
  );
  thread_start(&brian_thread);

  thread_t james_thread;

  thread_init(
    &james_thread,
    james_thread_stack,
    THREAD_STACK_SIZE,
    james_thread_routine,
    NULL
  );
  thread_start(&james_thread);

  thread_t sarah_thread;

  thread_init(
    &sarah_thread,
    sarah_thread_stack,
    THREAD_STACK_SIZE,
    sarah_thread_routine,
    NULL
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
