#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
* @addtogroup util
* @{
*/

/**
* @defgroup cpu CPU
* @brief CPU-related utilities.
* @{
*/

/**
* @brief Returns the identifier of the current CPU core.
* @return CPU core identifier.
*/
uint32_t cpu_get_id(void);

/**
* @brief Triggers a processor breakpoint.
* Causes execution to halt when a debugger is attached.
* @note The behavior when no debugger is attached is target-dependent.
*/
void cpu_breakpoint(void);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif
