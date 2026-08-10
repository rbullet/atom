#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <atom.h>

#define ALICE_THREAD_STACK_SIZE 1024

static uint32_t alice_thread_stack[ALICE_THREAD_STACK_SIZE];
static char blackboard[256];

static void* alice_thread_routine(void* const arg)
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
