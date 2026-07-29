#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "util/time/duration.h"

/**
 * @defgroup util Util
 * @{
 */

/**
 * @defgroup time Time
 * @brief Time representation, duration handling, and clock abstractions.
 * @{
 */

/**
 * @defgroup timestamp Timestamp
 * @brief Timestamp representation and operations.
 * @ingroup time
 * @{
 */

/**
 * @brief Represents a point in time.
 *
 * A timestamp is an opaque value produced by a clock source.
 * Its internal representation, precision, epoch and time unit are entirely
 * defined by the clock implementation.
 *
 * Applications must treat timestamps as opaque values and only manipulate
 * them through this API.
 *
 * Timestamps originating from different clock sources must never be compared
 * or combined.
 */
typedef struct timestamp_t
{
  uint64_t value; ///< Opaque clock-specific value.
} timestamp_t;

/**
 * @brief Get the current timestamp.
 *
 * Returns the current time according to the active clock implementation.
 *
 * The default implementation is platform-defined and may be overridden by the
 * application.
 *
 * @return Current timestamp.
 */
timestamp_t timestamp_now(void);

/**
 * @brief Create a timestamp offset by a duration.
 *
 * Returns the timestamp obtained by adding the specified duration to an
 * existing timestamp.
 *
 * @param timestamp Base timestamp.
 * @param duration Duration to add.
 *
 * @return Resulting timestamp.
 *
 * @pre The timestamp must originate from the active clock source.
 */
timestamp_t timestamp_add(timestamp_t timestamp, duration_t duration);

/**
 * @brief Determine whether a timestamp is before a reference timestamp.
 *
 * The first parameter is always the reference timestamp.
 *
 * @param first Reference timestamp.
 * @param second Timestamp being tested.
 *
 * @retval true  If @p second occurs before @p first.
 * @retval false Otherwise.
 *
 * @pre Both timestamps must originate from the same clock source.
 */
bool timestamp_is_before(timestamp_t first, timestamp_t second);

/**
 * @brief Determine whether a timestamp is before or equal to a reference timestamp.
 *
 * The first parameter is always the reference timestamp.
 *
 * @param first Reference timestamp.
 * @param second Timestamp being tested.
 *
 * @retval true  If @p second occurs before or at @p first.
 * @retval false Otherwise.
 *
 * @pre Both timestamps must originate from the same clock source.
 */
bool timestamp_is_before_or_equal(timestamp_t first, timestamp_t second);

/**
 * @brief Determine whether a timestamp is after a reference timestamp.
 *
 * The first parameter is always the reference timestamp.
 *
 * @param first Reference timestamp.
 * @param second Timestamp being tested.
 *
 * @retval true  If @p second occurs after @p first.
 * @retval false Otherwise.
 *
 * @pre Both timestamps must originate from the same clock source.
 */
bool timestamp_is_after(timestamp_t first, timestamp_t second);

/**
 * @brief Determine whether a timestamp is after or equal to a reference timestamp.
 *
 * The first parameter is always the reference timestamp.
 *
 * @param first Reference timestamp.
 * @param second Timestamp being tested.
 *
 * @retval true  If @p second occurs after or at @p first.
 * @retval false Otherwise.
 *
 * @pre Both timestamps must originate from the same clock source.
 */
bool timestamp_is_after_or_equal(timestamp_t first, timestamp_t second);

/**
 * @brief Determine whether a deadline has expired.
 *
 * Equivalent to testing whether the current timestamp is after or equal to
 * the specified deadline.
 *
 * @param deadline Deadline timestamp.
 *
 * @retval true  If the deadline has been reached or passed.
 * @retval false Otherwise.
 *
 * @pre The deadline must originate from the active clock source.
 */
bool timestamp_is_expired(timestamp_t deadline);

/**
 * @brief Compute the elapsed duration since a timestamp.
 *
 * Returns the duration between the supplied timestamp and the current time.
 *
 * @param timestamp Starting timestamp.
 *
 * @return Elapsed duration.
 *
 * @pre The timestamp must originate from the active clock source.
 */
duration_t timestamp_duration_since(timestamp_t timestamp);

/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif