#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "util/helpers.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup spinlock Spinlocks
 * @brief Busy-wait synchronization primitives.
 *
 * Provides lightweight locking primitives for protecting shared data
 * accessed concurrently from multiple callers.
 *
 * The implementation is architecture-defined and may rely on
 * architecture-specific synchronization mechanisms.
 *
 * Spinlocks are intended for extremely short critical sections and do not
 * block the caller. For synchronization requiring blocking, use higher-level
 * primitives such as mutexes, semaphores, condition variables, or event flags.
 *
 * @ingroup concurrent
 * @{
 */

/**
 * @brief Spinlock object.
 *
 * Represents a synchronization primitive used to serialize access to
 * shared data.
 *
 * Spinlocks are initialized to the unlocked state by zero initialization
 * or by using #SPINLOCK_INITIALIZER.
 */
typedef struct
{
  volatile uint32_t locked;
} spinlock_t;

#define SPINLOCK_UNLOCKED 0u
#define SPINLOCK_LOCKED   1u

/**
 * @brief Static spinlock initializer.
 */
#define SPINLOCK_INITIALIZER ((spinlock_t){ .locked = SPINLOCK_UNLOCKED })

/**
 * @brief Acquire a spinlock.
 *
 * Busy-waits until the spinlock becomes available and acquires it.
 *
 * This function is intended for extremely short critical sections.
 * The caller must not perform operations that may block while holding
 * the spinlock.
 *
 * @param spinlock Pointer to the spinlock to acquire.
 *
 * @post The caller owns the spinlock.
 *
 * @note Safe to call from both thread and interrupt context.
 *
 * @warning Do not call thread_sleep(), thread_yield(), or perform any
 * blocking operation while holding a spinlock.
 *
 * @warning Long critical sections increase contention and reduce
 * concurrency.
 */
void spinlock_lock(spinlock_t* spinlock);


/**
 * @brief Release a spinlock.
 *
 * Releases a spinlock previously acquired by the caller.
 *
 * @param spinlock Pointer to the spinlock to release.
 *
 * @pre The caller must currently own the spinlock.
 *
 * @post The spinlock becomes available for acquisition.
 *
 * @note Safe to call from both thread and interrupt context.
 *
 * @warning Releasing a spinlock that is not owned by the caller results
 * in undefined behavior.
 */
void spinlock_unlock(spinlock_t* spinlock);


/**
 * @brief Attempt to acquire a spinlock.
 *
 * Attempts to acquire the spinlock without blocking.
 *
 * @param spinlock Pointer to the spinlock to acquire.
 *
 * @retval true  The spinlock was successfully acquired.
 * @retval false The spinlock is currently owned.
 *
 * @post When true is returned, the caller owns the spinlock.
 *
 * @note Safe to call from both thread and interrupt context.
 */
bool spinlock_try_lock(spinlock_t* spinlock);


/**
 * @cond INTERNAL
 */

static inline void spinlock_auto_unlock(spinlock_t** spinlock)
{
  spinlock_unlock(*spinlock);
}

#define _WITH_SPINLOCK_BLOCK_WITH_ID(spinlock, ID)                               \
for (bool _CAT(_once_, ID) = true; _CAT(_once_, ID); _CAT(_once_, ID) = false)   \
    for (spinlock_t* __attribute__((cleanup(spinlock_auto_unlock)))              \
        _CAT(_spinlock_guard_, ID) = (spinlock_lock(spinlock), spinlock);        \
        _CAT(_once_, ID);                                                        \
        _CAT(_once_, ID) = false                                                 \
  )

/**
 * @endcond
 */


/**
 * @brief Scoped spinlock critical section.
 *
 * Acquires the spinlock when entering the scope and automatically releases
 * it when leaving.
 *
 * This helper relies on the GCC cleanup attribute.
 *
 * Example:
 *
 * @code
 * static spinlock_t lock = SPINLOCK_INITIALIZER;
 *
 * WITH_SPINLOCK(&lock)
 * {
 *     shared_state++;
 * }
 * @endcode
 *
 * The spinlock is released automatically when the scope exits, including
 * when leaving through an early return.
 *
 * @warning Critical sections protected by a spinlock should remain
 * extremely short.
 *
 * @warning Do not:
 * - sleep
 * - yield
 * - wait on synchronization primitives
 * - perform blocking operations
 * while holding a spinlock.
 */
#define WITH_SPINLOCK(spinlock) _WITH_SPINLOCK_BLOCK_WITH_ID(spinlock, __COUNTER__)


/** @} */ /* end of spinlock */
/** @} */ /* end of concurrent */


#ifdef __cplusplus
}
#endif