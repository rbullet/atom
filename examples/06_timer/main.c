#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define GPIO_LED_PIN 25

static void blink_led(void* arg)
{
  uint32_t const pin = (uint32_t)(uintptr_t)arg;
  gpio_toggle(pin);
}

int main(void)
{
  gpio_config(GPIO_LED_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);

  deferred_task_t deferred_task;

  deferred_task_start_periodic(
    &deferred_task,
    duration_of(0, SECONDS), // Initial delay
    duration_of(1, SECONDS), // Period between each invocation
    blink_led, // Callback
    (void*)GPIO_LED_PIN
  );

  while (1)
  {
  }

  return 0;
}
