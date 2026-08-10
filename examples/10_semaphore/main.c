#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define PRODUCER_THREAD_STACK_SIZE 1024
#define CONSUMER_THREAD_STACK_SIZE 1024

static uint32_t producer_thread_stack[PRODUCER_THREAD_STACK_SIZE];
static uint32_t consumer_thread_stack[CONSUMER_THREAD_STACK_SIZE];

static semaphore_t event_semaphore = SEMAPHORE_INITIALIZER(0);

static void* producer_routine(void* const arg)
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

static void* consumer_routine(void* const arg)
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