#include <assert.h>
#include <stdbool.h>

#include <atom.h>

duration_t duration_convert_to(duration_t const duration, time_unit_t const unit)
{
  float milliseconds;

  switch (duration.unit)
  {
  case MILLISECONDS:
    milliseconds = duration.value;
    break;

  case SECONDS:
    milliseconds = duration.value * 1000;
    break;

  case MINUTES:
    milliseconds = duration.value * 60 * 1000;
    break;

  case HOURS:
    milliseconds = duration.value * 60 * 60 * 1000;
    break;

  default:
    assert(false && "Invalid time unit");
  }

  switch (unit)
  {
  case MILLISECONDS:
    return duration_of(milliseconds, MILLISECONDS);

  case SECONDS:
    return duration_of(milliseconds / 1000, SECONDS);

  case MINUTES:
    return duration_of(milliseconds / (60 * 1000), MINUTES);

  case HOURS:
    return duration_of(milliseconds / (60 * 60 * 1000), HOURS);

  default:
    assert(false && "Invalid time unit");
  }
}
