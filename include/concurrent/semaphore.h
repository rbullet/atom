#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "util/helpers.h"
#include "util/collection/list.h"
#include "concurrent/spinlock.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup semaphore Semaphores
 *
 * @brief Lightweight counting semaphores for thread synchronization.
 *
 * A semaphore maintains a count of available permits. Threads acquire
 * permits before accessing a shared resource and release them when the
 * resource is no longer needed.
 *
 * If no permit is available, the calling thread blocks until another
 * thread releases one. Woken threads compete normally to acquire an
 * available permit; resuming a thread does not guarantee that it will
 * acquire the semaphore before another runnable thread.
 *
 * Semaphores have no ownership. Any thread may release a semaphore,
 * regardless of which thread previously acquired it.
 *
 * @{
 */

/**
 * @brief Semaphore object.
 *
 * A semaphore maintains a count of available permits together with the
 * queue of threads currently waiting for one.
 */
typedef struct semaphore_t
{
  /** @cond INTERNAL */
  spinlock_t spinlock; ///< Spinlock used for protecting the semaphore state.
  uint32_t permits;    ///< Number of available permits.
  list_t waiters;      ///< Threads waiting for a permit.
  /** @endcond */
} semaphore_t;


/**
* @brief Statically initializes a semaphore.
 *
 * Initializes a semaphore with the specified number of permits.
 *
 * The internal spinlock is allocated automatically on first use.
 *
 * Example:
 *
 * @code
 * semaphore_t resource_semaphore = SEMAPHORE_INITIALIZER(3);
 * @endcode
 */
#define SEMAPHORE_INITIALIZER(p)      \
  ((semaphore_t){                     \
    .spinlock = SPINLOCK_INITIALIZER, \
    .permits = (p),                   \
    .waiters = LIST_INITIALIZER       \
  })


/**
 * @brief Acquires a semaphore permit.
 *
 * If one or more permits are available, one permit is consumed and the
 * function returns immediately.
 *
 * Otherwise, the calling thread blocks until a permit becomes available.
 *
 * @param semaphore Semaphore instance.
 */
void semaphore_acquire(semaphore_t* semaphore);


/**
 * @brief Attempts to acquire a semaphore within a timeout.
 *
 * Blocks until a permit becomes available or the specified timeout expires.
 *
 * @param semaphore Semaphore to acquire.
 * @param timeout Maximum duration to wait for a permit.
 *
 * @return true if a permit was acquired, false if the timeout expired.
 *
 * @pre Must be called from thread context.
 */
bool semaphore_acquire_with_timeout(semaphore_t* semaphore, duration_t timeout);


/**
 * @brief Attempts to acquire a semaphore permit without blocking.
 *
 * If a permit is available, it is consumed and the function returns
 * immediately. Otherwise, the semaphore is left unchanged.
 *
 * @param semaphore Semaphore instance.
 *
 * @return true if a permit was acquired, false otherwise.
 */
bool semaphore_try_acquire(semaphore_t* semaphore);


/**
 * @brief Releases a semaphore permit.
 *
 * Increments the number of available permits and resumes one waiting
 * thread, if any.
 *
 * Releasing a permit does not transfer ownership to a specific thread.
 * A resumed thread competes normally with other runnable threads to
 * acquire the newly available permit.
 *
 * @param semaphore Semaphore instance.
 */
void semaphore_release(semaphore_t* semaphore);


/**
 * @brief Returns the current number of available permits.
 *
 * @param semaphore Semaphore instance.
 *
 * @return Snapshot of the current permit count.
 */
uint32_t semaphore_count(semaphore_t const* semaphore);


/**
 * @cond INTERNAL
 */

static inline void semaphore_auto_release(semaphore_t** semaphore)
{
  semaphore_release(*semaphore);
}

/**
 * @endcond
 */

/**
 * @brief Acquires a semaphore for the duration of a scope.
 *
 * The semaphore is automatically released when leaving the scope,
 * including when exiting via `return`.
 *
 * Example:
 *
 * @code
 * WITH_SEMAPHORE(&pool_semaphore)
 * {
 *     use_pooled_resource();
 * }
 * WITH_SEMAPHORE_END
 * @endcode
 */
#define WITH_SEMAPHORE(semaphore)                                         \
    (void)({                                                              \
        semaphore_t* __semaphore__                                        \
            __attribute__((cleanup(semaphore_auto_release)))              \
            = (semaphore_acquire(semaphore), (semaphore));

#define WITH_SEMAPHORE_END (void)0;})


/** @} */

/** @} */

#ifdef __cplusplus
}
#endif