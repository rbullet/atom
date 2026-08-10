#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "util/cpu.h"

/**
 * @addtogroup util
 * @{
 */

/**
 * @defgroup assert Assertions
 * @brief Runtime assertion support.
 *
 * Assertions are enabled in debug builds and disabled in release builds.
 *
 * @{
 */

/**
 * @brief Checks a condition and reports assertion failures.
 *
 * If @p condition evaluates to false, the assertion failure is reported
 * through the ATOM logging system and execution enters the processor
 * breakpoint handler.
 *
 * @param condition Condition that must evaluate to true.
 * @param fmt printf-style format string for the failure message.
 * @param ... Arguments for the format string.
 *
 * @note Assertions are only active when `DEBUG` is defined.
 */
#ifdef DEBUG

#define ATOM_ASSERT(condition, fmt, ...) \
do                                                                          \
{                                                                           \
    if (!(condition))                                                       \
    {                                                                       \
        log_fatal("Assertion '%s' failed: " fmt,                            \
                  #condition,                                               \
                  ##__VA_ARGS__);                                           \
        cpu_breakpoint();                                                   \
        for (;;);                                                           \
    }                                                                       \
} while (0)
#else

#define ATOM_ASSERT(condition, fmt, ...) ((void)0)

#endif

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif