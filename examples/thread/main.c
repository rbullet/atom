#include <stdbool.h>

#include "atom.h"

#define LED_PIN 25

static thread_t worker;
static uint32_t worker_stack[256];

static thread_t led_blink_thread;
static uint32_t led_blink_thread_stack[256];

static void* worker_thread(void* arg)
{
  (void)arg;

  while (true)
  {
    log_info("Hello from worker");
    thread_sleep(duration_of(1, SECONDS));
  }

  return NULL;
}

static void* blink_led_thread(void* arg)
{
  uint32_t pin = (uint32_t)(uintptr_t)arg;
  while (true)
  {
    gpio_toggle(pin);
    thread_sleep(duration_of(1, SECONDS));
  }

  return NULL;
}

int main(void)
{
  // main() already executes as a schedulable thread.

  gpio_config(LED_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);

  thread_init(&led_blink_thread, led_blink_thread_stack, sizeof(led_blink_thread_stack), blink_led_thread, (void*)(uintptr_t)LED_PIN);
  thread_start(&led_blink_thread);

  thread_init(&worker, worker_stack, sizeof(worker_stack), worker_thread, NULL);
  thread_start(&worker);

  while (true)
  {
    log_info("Hello from main");
    thread_sleep(duration_of(5, SECONDS));
  }
}
