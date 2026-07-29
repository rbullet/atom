#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "util/helpers.h"
#include "util/collection/list.h"

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
  uint32_t permits;   ///< Number of available permits.
  list_t waiters;     ///< Threads waiting for a permit.
} semaphore_t;

/**
 * @brief Statically initializes a semaphore.
 *
 * @param p Initial number of available permits.
 */
#define SEMAPHORE_INITIALIZER(p) \
  ((semaphore_t){                \
    .permits = (p),              \
    .waiters = LIST_INITIALIZER  \
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
 * @return Current number of available permits.
 */
uint32_t semaphore_count(semaphore_t const* semaphore);

/**
 * @cond INTERNAL
 */

static inline void semaphore_auto_unlock(semaphore_t** semaphore)
{
  semaphore_release(*semaphore);
}

#define _WITH_SEMAPHORE_BLOCK_WITH_ID(semaphore, ID)                             \
for (bool _CAT(_once_, ID) = true; _CAT(_once_, ID); _CAT(_once_, ID) = false)   \
  for (semaphore_t* __attribute__((cleanup(semaphore_auto_unlock)))              \
      _CAT(_semaphore_guard_, ID) = (semaphore_acquire(semaphore), semaphore);   \
      _CAT(_once_, ID);                                                          \
      _CAT(_once_, ID) = false                                                   \
  )

/**
 * @endcond
 */

/**
 * @brief Acquires a semaphore for the duration of a scope.
 *
 * The semaphore is automatically released when leaving the scope,
 * including when exiting via `return`.
 */
#define WITH_SEMAPHORE(semaphore) _WITH_SEMAPHORE_BLOCK_WITH_ID(semaphore, __COUNTER__)

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif