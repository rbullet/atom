#include "internal.h"

timestamp_t scheduler_timestamp_now(void)
{
  return (timestamp_t){.value = sys_tick};
}

timestamp_t scheduler_timestamp_add(timestamp_t const timestamp, duration_t const duration)
{
  return (timestamp_t){.value = timestamp.value + (uint64_t)duration_convert_to(duration, MILLISECONDS).value};
}

bool scheduler_timestamp_is_before(timestamp_t const first, timestamp_t const second)
{
  return (int64_t)(first.value - second.value) < 0;
}

bool scheduler_timestamp_is_before_or_equal(timestamp_t const first, timestamp_t const second)
{
  return (int64_t)(first.value - second.value) <= 0;
}

bool scheduler_timestamp_is_after(timestamp_t const first, timestamp_t const second)
{
  return (int64_t)(first.value - second.value) > 0;
}

bool scheduler_timestamp_is_after_or_equal(timestamp_t const first, timestamp_t const second)
{
  return (int64_t)(first.value - second.value) >= 0;
}

bool scheduler_timestamp_is_expired(timestamp_t const deadline)
{
  return (int64_t)(sys_tick - deadline.value) >= 0;
}

duration_t scheduler_timestamp_duration_since(timestamp_t const timestamp)
{
  return duration_of((float)(sys_tick - timestamp.value), MILLISECONDS);
}
