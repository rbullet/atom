#include "rp2040/atom.h"

timestamp_t __attribute__((weak)) timestamp_now(void)
{
  return scheduler_timestamp_now();
}

timestamp_t __attribute__((weak)) timestamp_add(timestamp_t const timestamp, duration_t const duration)
{
  return scheduler_timestamp_add(timestamp, duration);
}

bool __attribute__((weak)) timestamp_is_before(timestamp_t const first, timestamp_t const second)
{
  return scheduler_timestamp_is_before(first, second);
}

bool __attribute__((weak)) timestamp_is_before_or_equal(timestamp_t const first, timestamp_t const second)
{
  return scheduler_timestamp_is_before_or_equal(first, second);
}

bool __attribute__((weak)) timestamp_is_after(timestamp_t const first, timestamp_t const second)
{
  return scheduler_timestamp_is_after(first, second);
}

bool __attribute__((weak)) timestamp_is_after_or_equal(timestamp_t const first, timestamp_t const second)
{
  return scheduler_timestamp_is_after_or_equal(first, second);
}

bool __attribute__((weak)) timestamp_is_expired(timestamp_t const deadline)
{
  return scheduler_timestamp_is_expired(deadline);
}

duration_t __attribute__((weak)) timestamp_duration_since(timestamp_t const timestamp)
{
  return scheduler_timestamp_duration_since(timestamp);
}
