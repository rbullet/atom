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
 * @brief Hardware-backed cross-core synchronization primitives.
 *
 * Provides busy-wait locking primitives based on the RP2040 hardware
 * spinlock peripheral.
 *
 * Spinlocks are intended for very short critical sections shared between
 * cores or between interrupt and thread contexts.
 *
 * Spinlocks do not provide thread blocking. For thread-to-thread
 * synchronization, use higher-level primitives such as mutexes or
 * notifications.
 *
 * @ingroup concurrent
 * @{
 */


/**
 * @brief RP2040 hardware spinlock type.
 *
 * A spinlock is represented by a volatile 32-bit hardware register.
 *
 * On RP2040:
 * - Reading a spinlock register attempts to acquire the lock.
 * - A non-zero value indicates successful acquisition.
 * - Zero indicates that another core currently owns the lock.
 *
 * The lock ownership is tracked by the hardware and is associated with
 * the executing core.
 */
typedef volatile uint32_t spinlock_t;


/**
 * @brief Acquire a spinlock.
 *
 * Busy-waits until the lock becomes available.
 *
 * This operation is intended for short critical sections only. The caller
 * must not perform operations that may block while holding the lock.
 *
 * @param lock Pointer to a hardware spinlock register.
 *
 * @pre lock must reference a valid RP2040 hardware spinlock.
 *
 * @post The calling core owns the spinlock.
 *
 * @note Safe from thread and interrupt context.
 *
 * @warning Do not call thread_sleep(), thread_yield(), or any blocking
 *          operation while holding a spinlock.
 *
 * @warning Long critical sections can stall the other core.
 */
void spinlock_lock(spinlock_t* lock);


/**
 * @brief Release a spinlock.
 *
 * Releases a lock previously acquired by the calling core.
 *
 * @param lock Pointer to a hardware spinlock register.
 *
 * @pre The calling core must currently own the spinlock.
 *
 * @post The spinlock becomes available to other cores.
 *
 * @note Safe from thread and interrupt context.
 *
 * @warning Releasing an unowned spinlock results in undefined behavior.
 */
void spinlock_unlock(spinlock_t* lock);


/**
 * @brief Try to acquire a spinlock.
 *
 * Attempts to acquire the lock and returns immediately.
 *
 * @param lock Pointer to a hardware spinlock register.
 *
 * @retval true  The lock was acquired by the calling core.
 * @retval false The lock is already owned by another core.
 *
 * @pre lock must reference a valid RP2040 hardware spinlock.
 *
 * @post When true is returned, the calling core owns the lock.
 *
 * @note Safe from thread and interrupt context.
 */
bool spinlock_try_lock(spinlock_t* lock);


/**
 * @cond INTERNAL
 */

static inline void spinlock_auto_unlock(spinlock_t** lock)
{
  spinlock_unlock(*lock);
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
 * Acquires the spinlock when entering the block and automatically releases
 * it when leaving the block.
 *
 * This helper relies on the GCC cleanup attribute.
 *
 * Example:
 *
 * @code
 * WITH_SPINLOCK(scheduler_spinlock)
 * {
 *     shared_state++;
 * }
 * @endcode
 *
 * The spinlock is released automatically at the end of the scope, including
 * when leaving through an early return.
 *
 * @warning The protected section must remain extremely short.
 *
 * @warning Do not:
 * - sleep
 * - yield
 * - wait on a mutex
 * - perform blocking operations
 * while holding a spinlock.
 */
#define WITH_SPINLOCK(spinlock) _WITH_SPINLOCK_BLOCK_WITH_ID(spinlock, __COUNTER__)


/** @} */ /* end of spinlock */
/** @} */ /* end of concurrent */


#ifdef __cplusplus
}
#endif