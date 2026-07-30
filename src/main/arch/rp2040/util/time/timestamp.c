#include <atom.h>
#include "rp2040/concurrent/scheduler.h"

__attribute__((weak)) timestamp_t timestamp_now(void)
{
  return scheduler_timestamp_now();
}

__attribute__((weak)) timestamp_t timestamp_add(timestamp_t const timestamp, duration_t const duration)
{
  return scheduler_timestamp_add(timestamp, duration);
}

__attribute__((weak)) bool timestamp_is_before(timestamp_t const first, timestamp_t const second)
{
  return scheduler_timestamp_is_before(first, second);
}

__attribute__((weak)) bool timestamp_is_before_or_equal(timestamp_t const first, timestamp_t const second)
{
  return scheduler_timestamp_is_before_or_equal(first, second);
}

__attribute__((weak)) bool timestamp_is_after(timestamp_t const first, timestamp_t const second)
{
  return scheduler_timestamp_is_after(first, second);
}

__attribute__((weak)) bool timestamp_is_after_or_equal(timestamp_t const first, timestamp_t const second)
{
  return scheduler_timestamp_is_after_or_equal(first, second);
}

__attribute__((weak)) bool timestamp_is_expired(timestamp_t const deadline)
{
  return scheduler_timestamp_is_expired(deadline);
}

__attribute__((weak)) duration_t timestamp_duration_since(timestamp_t const timestamp)
{
  return scheduler_timestamp_duration_since(timestamp);
}
