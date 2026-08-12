#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "util/helpers.h"
#include "util/collection/list.h"
#include "concurrent/spinlock.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup mutex Mutex
 * @brief Scheduler-aware reentrant mutual exclusion primitives.
 *
 * Provides blocking mutexes for synchronizing cooperative threads.
 *
 * Mutexes are reentrant: the thread owning a mutex may acquire it multiple
 * times. Each successful acquisition increments the ownership depth and must
 * be matched by a corresponding call to mutex_unlock().
 *
 * Unlike spinlocks, mutexes do not busy-wait while contended. When a mutex
 * is already owned by another thread, the caller is blocked and another
 * runnable thread may execute.
 *
 * When a mutex is released, one waiting thread is resumed, if any.
 * Resuming a thread does not transfer ownership of the mutex. Resumed
 * threads compete normally with other runnable threads to acquire the
 * mutex.
 *
 * Mutexes are intended for thread-to-thread synchronization. For short
 * critical sections shared between cores or interrupt contexts, use
 * spinlocks instead.
 *
 * @ingroup concurrent
 * @{
 */


/**
 * @brief Forward declaration of thread control block.
 */
typedef struct thread_t thread_t;


/**
 * @brief Mutex control block.
 *
 * Mutex objects are owned by the application and must remain valid while
 * they are used by any thread.
 *
 * The internal fields are managed by the mutex implementation and must not
 * be modified directly.
 *
 * A mutex tracks:
 * - whether it is locked,
 * - the current owning thread,
 * - the recursive ownership depth,
 * - threads waiting to acquire it.
 */
typedef struct mutex_t
{
  /** @cond INTERNAL */
  spinlock_t spinlock;  ///< Spinlock.
  uint32_t count;       ///< Recursive acquisition depth.
  thread_t* owner;      ///< Thread currently owning the mutex.
  list_t waiters;       ///< Threads waiting to acquire the mutex.
  /** @endcond */
} mutex_t;


/**
 * @brief Static mutex initializer.
 *
 * Initializes a mutex in the unlocked state.
 *
 * The internal spinlock is allocated automatically on first use.
 *
 * Example:
 *
 * @code
 * mutex_t resource_lock = MUTEX_INITIALIZER;
 * @endcode
 */
#define MUTEX_INITIALIZER \
  ((mutex_t){ \
    .spinlock = SPINLOCK_INITIALIZER, \
    .count = 0, \
    .owner = NULL, \
    .waiters = LIST_INITIALIZER \
  })


/**
 * @brief Acquires a mutex.
 *
 * If the mutex is currently owned by another thread, the calling thread
 * blocks until it is resumed and successfully acquires the mutex.
 *
 * A resumed thread is not guaranteed to acquire the mutex immediately.
 * After being resumed, it competes normally with other runnable threads
 * until ownership is successfully obtained.
 *
 * If the calling thread already owns the mutex, the acquisition succeeds
 * immediately and increments the recursive ownership depth.
 *
 * Each successful call to mutex_lock() must be paired with a matching
 * call to mutex_unlock().
 *
 * @param mutex Mutex to acquire.
 *
 * @pre mutex must point to a valid initialized mutex.
 *
 * @post The calling thread owns the mutex.
 *
 * @note May block and perform a context switch.
 *
 * @warning Must only be called from thread context.
 * @warning Must not be called from interrupt context.
 */
void mutex_lock(mutex_t* mutex);


/**
 * @brief Attempts to acquire a mutex within a timeout.
 *
 * Blocks until the mutex is acquired or the specified timeout expires.
 *
 * @param mutex Mutex to acquire.
 * @param timeout Maximum duration to wait for the mutex.
 *
 * @return true if the mutex was acquired, false if the timeout expired.
 *
 * @pre Must be called from thread context.
 */
bool mutex_lock_with_timeout(mutex_t* mutex, duration_t timeout);


/**
 * @brief Releases a mutex.
 *
 * Decrements the recursive ownership depth.
 *
 * The mutex is released only when the ownership depth reaches zero.
 * At that point, one waiting thread is resumed, if any.
 *
 * Resuming a thread does not transfer ownership of the mutex. The
 * resumed thread competes normally with other runnable threads to
 * acquire the mutex.
 *
 * @param mutex Mutex to release.
 *
 * @pre The calling thread must own the mutex.
 *
 * @post The mutex becomes available when the ownership depth reaches zero.
 *
 * @note May wake another thread.
 *
 * @warning Must only be called from thread context.
 * @warning Releasing a mutex not owned by the caller is undefined behavior.
 */
void mutex_unlock(mutex_t* mutex);


/**
 * @brief Attempts to acquire a mutex without blocking.
 *
 * If the calling thread already owns the mutex, the acquisition succeeds
 * immediately and increments the recursive ownership depth.
 *
 * @param mutex Mutex to acquire.
 *
 * @retval true  Mutex acquired by the calling thread.
 * @retval false Mutex currently owned by another thread.
 *
 * @pre mutex must point to a valid initialized mutex.
 *
 * @post If true is returned, the caller owns the mutex.
 *
 * @note Must only be called from thread context.
 */
bool mutex_try_lock(mutex_t* mutex);


/**
 * @cond INTERNAL
 */

static inline void mutex_auto_unlock(mutex_t** mutex)
{
  mutex_unlock(*mutex);
}

#define _WITH_MUTEX_BLOCK_WITH_ID(mutex, ID)                                     \
for (bool _CAT(_once_, ID) = true; _CAT(_once_, ID); _CAT(_once_, ID) = false)   \
  for (mutex_t* __attribute__((cleanup(mutex_auto_unlock)))                      \
      _CAT(_mutex_guard_, ID) = (mutex_lock(mutex), mutex);                      \
      _CAT(_once_, ID);                                                          \
      _CAT(_once_, ID) = false                                                   \
  )

/**
 * @endcond
 */


/**
 * @brief Scoped mutex critical section.
 *
 * Acquires the mutex on entry and automatically releases it when leaving
 * the scope, including when exiting through `return`.
 *
 * Reentrant acquisition is supported, so nested WITH_MUTEX blocks from the
 * same thread are allowed.
 *
 * Example:
 *
 * @code
 * WITH_MUTEX(&resource_lock)
 * {
 *     update_shared_state();
 * }
 * @endcode
 *
 * @warning Must not be used from interrupt context.
 * @warning Do not sleep indefinitely while holding a mutex.
 */
#define WITH_MUTEX(mutex) _WITH_MUTEX_BLOCK_WITH_ID(mutex, __COUNTER__)


/** @} */ /* end of mutex */
/** @} */ /* end of concurrent */

#ifdef __cplusplus
}
#endif