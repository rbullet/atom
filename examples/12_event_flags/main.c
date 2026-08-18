#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define THREAD_STACK_SIZE 1024

static uint32_t annie_thread_stack[THREAD_STACK_SIZE];
static uint32_t james_thread_stack[THREAD_STACK_SIZE];
static uint32_t brian_thread_stack[THREAD_STACK_SIZE];


#define CONCERT_STARTED (1u << 0)
#define BRIAN_PRESENT (1u << 1)

static event_flags_t concert_events = EVENT_FLAGS_INITIALIZER;

static void* annie_thread_routine(void* arg)
{
  printf("Annie: I'm waiting the beginning of the concert!\r\n");
  event_flags_wait(&concert_events, CONCERT_STARTED, EVENT_FLAGS_ALL_SET);
  printf("Annie: The concert has started!\r\n");
  return NULL;
}

static void* brian_thread_routine(void* arg)
{
  printf("Brian: I'm waiting the beginning of the concert!\r\n");
  event_flags_wait(&concert_events, CONCERT_STARTED, EVENT_FLAGS_ALL_SET);
  printf("Brian: The concert has started! Let me warn James!\r\n");
  event_flags_set(&concert_events, BRIAN_PRESENT);
  return NULL;
}

static void* james_thread_routine(void* arg)
{
  printf("James: I'm waiting for Brian to arrive!\r\n");
  event_flags_wait(&concert_events, CONCERT_STARTED | BRIAN_PRESENT, EVENT_FLAGS_ALL_SET);
  printf("James: Brian is here! Let's enjoy the concert!\r\n");
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

  printf("The concert will start soon...!\r\n");
  thread_sleep(duration_of(5, SECONDS));

  printf("The concert has started!\r\n");
  event_flags_set(&concert_events, CONCERT_STARTED);

  while (1)
  {
  }
  return 0;
}
