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
 * Provides busy-wait locking primitives backed by hardware spinlock
 * support on the target architecture.
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
 * @brief Hardware spinlock type.
 *
 * A spinlock represents an architecture-provided atomic locking primitive.
 *
 * The exact acquisition and release semantics are defined by the target
 * architecture implementation.
 */
typedef volatile uint32_t spinlock_t;

/**
 * @brief Spinlock reservation policy.
 */
typedef enum
{
  /**
   * @brief Reserves a spinlock exclusively for the caller.
   *
   * The returned spinlock is guaranteed not to be shared with any other
   * reservation until it is returned to the pool.
   *
   * If no exclusive spinlock is available, the reservation fails and an
   * assertion is raised.
   */
  SPINLOCK_EXCLUSIVE,

  /**
   * @brief Reserves a spinlock from the shared pool.
   *
   * Pooled spinlocks are intended for lightweight synchronization objects
   * where occasional contention between unrelated resources is acceptable.
   *
   * Multiple logical synchronization objects may share the same hardware
   * spinlock. The allocator balances usage between available pooled locks.
   */
  SPINLOCK_POOLED

} spinlock_reservation_t;

/**
 * @brief Lazily initializes a spinlock resource.
 *
 * Allocates a spinlock from the requested reservation class if the pointer
 * is currently NULL.
 *
 * This helper is intended for synchronization objects supporting static
 * initialization. The first use transparently acquires the required
 * hardware resource.
 *
 * @param lock Address of the spinlock pointer to initialize.
 * @param type Reservation policy.
 *
 * @pre lock must not be NULL.
 */
void spinlock_pool_ensure_initialized(spinlock_t** lock, spinlock_reservation_t type);

/**
 * @brief Reserves a spinlock from the pool.
 *
 * @param type Reservation policy.
 *
 * @return Reserved spinlock.
 *
 * @note Exclusive reservations are unique until returned to the pool.
 *
 * @note Pooled reservations may reuse spinlocks that are already assigned
 * to other pooled resources in order to minimize contention while avoiding
 * allocation failures.
 */
spinlock_t* spinlock_pool_reserve(spinlock_reservation_t type);


/**
 * @brief Returns a spinlock to the pool.
 *
 * Releases a reservation previously obtained from
 * spinlock_pool_reserve().
 *
 * For exclusive reservations, the hardware spinlock becomes available for
 * another exclusive allocation.
 *
 * For pooled reservations, the usage count is decremented so the allocator
 * can rebalance future pooled allocations.
 *
 * @param lock Spinlock to return.
 */
void spinlock_pool_return(spinlock_t const* lock);


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
 * WITH_SPINLOCK(spinlock0)
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
