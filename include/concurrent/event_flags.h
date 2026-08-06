#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "concurrent/spinlock.h"
#include "util/collection/list.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup event_flags Event Flags
 * @brief Bit-based thread synchronization primitive.
 *
 * Event flags allow threads to synchronize using a set of persistent
 * bit flags. A thread can wait until one or more conditions are satisfied.
 *
 * Flags remain set until explicitly cleared with event_flags_clear().
 * Multiple threads can wait on the same flags and all matching waiters
 * are resumed when the condition becomes true.
 *
 * @{
 */

/**
 * @brief Event flag bit mask type.
 *
 * Each bit represents an independent event condition.
 */
typedef uint32_t event_flags_mask_t;

/**
 * @brief Event flags object.
 *
 * Stores the current event state and the list of threads waiting
 * for matching conditions.
 */
typedef struct
{
  spinlock_t spinlock;
  event_flags_mask_t flags;
  list_t waiters;
} event_flags_t;

/**
 * @brief Event flags wait condition mode.
 */
typedef enum
{
  /**
   * @brief Wait until at least one requested flag is set.
   */
  EVENT_FLAGS_ANY_SET,

  /**
   * @brief Wait until all requested flags are set.
   */
  EVENT_FLAGS_ALL_SET
} event_flags_mode_t;

/**
 * @brief Initializes an event flags object.
 *
 * The object can be statically initialized using this macro.
 */
#define EVENT_FLAGS_INITIALIZER ((event_flags_t){ .spinlock = SPINLOCK_INITIALIZER, .flags = 0, .waiters = LIST_INITIALIZER })

/**
 * @brief Sets one or more event flags.
 *
 * Any thread waiting for a condition satisfied by the new flag state
 * is resumed.
 *
 * @param event Event flags object.
 * @param flags Flags to set.
 */
void event_flags_set(event_flags_t* event, event_flags_mask_t flags);

/**
 * @brief Clears one or more event flags.
 *
 * Cleared flags no longer satisfy wait conditions.
 *
 * @param event Event flags object.
 * @param flags Flags to clear.
 */
void event_flags_clear(event_flags_t* event, event_flags_mask_t flags);

/**
 * @brief Waits until an event condition is satisfied.
 *
 * If the condition is already true, the function returns immediately.
 * Otherwise, the current thread is blocked until another thread sets
 * the required flags.
 *
 * @param event Event flags object.
 * @param mask Flags to wait for.
 * @param mode Condition mode.
 */
void event_flags_wait(event_flags_t* event, event_flags_mask_t mask, event_flags_mode_t mode);

/**
 * @brief Tests an event condition without blocking.
 *
 * @param event Event flags object.
 * @param mask Flags to test.
 * @param mode Condition mode.
 *
 * @return true if the condition is satisfied, false otherwise.
 */
bool event_flags_try_wait(event_flags_t* event, event_flags_mask_t mask, event_flags_mode_t mode);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif