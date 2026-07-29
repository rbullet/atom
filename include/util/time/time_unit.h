#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup util Util
 * @{
 */

/**
 * @defgroup time Time
 * @{
 */

/**
 * @defgroup time_unit Time Unit
 * @brief Time unit definitions.
 * @ingroup time
 * @{
 */

/**
 * @brief Enumeration of supported time units.
 *
 * Defines the units that can be used to represent time-based values
 * throughout the library.
 */
typedef enum
{
  MILLISECONDS = 0, ///< Milliseconds (ms)
  SECONDS      = 1, ///< Seconds (s)
  MINUTES      = 2, ///< Minutes (min)
  HOURS        = 3, ///< Hours (h)
} time_unit_t;

/** @} */ /* end of time_unit */
/** @} */ /* end of time */
/** @} */ /* end of util */

#ifdef __cplusplus
}
#endif