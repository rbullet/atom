#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define GPIO_LED_PIN 25

#define HELLO_THREAD_STACK_SIZE 1024
#define BLINK_THREAD_STACK_SIZE 512

static uint32_t hello_thread_stack[HELLO_THREAD_STACK_SIZE];
static uint32_t blink_thread_stack[BLINK_THREAD_STACK_SIZE];

static void* logger_thread_routine(void* arg)
{
  (void)arg;
  while (1)
  {
    printf("Hello from thread!\r\n");
    thread_sleep(duration_of(1, SECONDS));
  }
  return NULL;
}

static void* blink_thread_routine(void* arg)
{
  uint32_t const pin = (uint32_t)(uintptr_t)arg;
  while (1)
  {
    gpio_toggle(pin);
    thread_sleep(duration_of(500, MILLISECONDS));
  }
  return NULL;
}

int main(void)
{
  gpio_config(GPIO_LED_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);

  thread_t hello_thread;

  thread_init(
    &hello_thread,
    hello_thread_stack,
    HELLO_THREAD_STACK_SIZE,
    logger_thread_routine,
    NULL
  );
  thread_start(&hello_thread);

  thread_t blink_thread;

  thread_init(
    &blink_thread,
    blink_thread_stack,
    BLINK_THREAD_STACK_SIZE,
    blink_thread_routine,
    (void *)(uintptr_t)GPIO_LED_PIN
  );
  thread_start(&blink_thread);

  while (1)
  {
    printf("Hello from main!\r\n");
    thread_sleep(duration_of(2, SECONDS));
  }

  return 0;
}
