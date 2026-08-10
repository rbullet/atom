#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "util/time/time_unit.h"

/**
* @defgroup util Util
* @{
*/

/**
* @defgroup time Time
* @{
*/

/**
 * @defgroup duration Duration
 * @brief Time duration representation and conversion utilities.
 * @ingroup time
 * @{
 */

/**
 * @brief Represents a time duration.
 *
 * A duration consists of a floating-point value associated with a time unit.
 * The value is not normalized, allowing equivalent durations to be represented
 * in different units (e.g. `1000 ms` and `1 s`).
 */
typedef struct
{
  float value; ///< Duration value.
  time_unit_t unit; ///< Unit associated with the value.
} duration_t;

#define DURATION_INITIALIZER(v, u) ((duration_t){.value = (v), .unit = (u)})

/**
 * @brief Creates a duration from a value and a time unit.
 *
 * Example:
 * @code
 * duration_t timeout = duration_of(500, MILLISECONDS);
 * duration_t period  = duration_of(2, SECONDS);
 * @endcode
 *
 * @param value Duration value.
 * @param unit Time unit associated with the value.
 *
 * @return A duration initialized with the specified value and unit.
 */
static inline duration_t duration_of(float const value, time_unit_t const unit)
{
  return DURATION_INITIALIZER(value, unit);
}

/**
 * @brief Converts a duration to another time unit.
 *
 * The returned duration represents the same amount of time expressed in the
 * requested unit.
 *
 * @param duration Duration to convert.
 * @param unit Target time unit.
 *
 * @return An equivalent duration expressed in the specified unit.
 *
 * @note Floating-point conversions may introduce rounding errors.
 */
duration_t duration_convert_to(duration_t duration, time_unit_t unit);

/** @} */ /* end of duration */
/** @} */ /* end of time */
/** @} */ /* end of util */

#ifdef __cplusplus
}
#endif
