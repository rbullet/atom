#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/mutex.h"
#include "util/collection/list.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup condition Condition Variables
 * @brief Thread synchronization primitives based on wait/notify semantics.
 *
 * Condition variables allow threads to sleep until a shared state changes.
 *
 * A condition variable is always associated with a mutex protecting the
 * shared state. Threads waiting on a condition must hold the associated
 * mutex before calling condition_wait().
 *
 * Condition variables do not store state. A notification sent while no thread is
 * waiting is lost.
 *
 * @ingroup concurrent
 * @{
 */

/**
 * @brief Condition variable object.
 *
 * Condition variables are owned by the application and must remain valid
 * while they are used by waiting threads.
 *
 * The internal fields must not be modified directly.
 */
typedef struct condition_variable_t
{
  list_t waiters;
} condition_variable_t;

/**
 * @brief Static condition variable initializer.
 *
 * Example:
 *
 * @code
 * condition_variable_t data_available = CONDITION_VARIABLE_INITIALIZER;
 * @endcode
 */
#define CONDITION_VARIABLE_INITIALIZER ((condition_variable_t){ .waiters = LIST_INITIALIZER })

/**
 * @brief Wait for a condition.
 *
 * Blocks the current thread until another thread signals or broadcasts the
 * condition variable.
 *
 * The calling thread must hold the mutex before entering this function.
 * The mutex is released while the thread is waiting and automatically
 * reacquired before this function returns.
 *
 * Example:
 *
 * @code
 * WITH_MUTEX(&lock)
 * {
 *     while (!data_ready)
 *     {
 *         condition_wait(&condition, &lock);
 *     }
 *
 *     consume_data();
 * }
 * @endcode
 *
 * @param condition Condition variable to wait on.
 * @param mutex Mutex protecting the shared state.
 *
 * @pre condition must be initialized.
 * @pre mutex must be initialized.
 * @pre The calling thread must own mutex.
 *
 * @post The calling thread owns mutex again before returning.
 *
 * @note May block and perform a context switch.
 *
 * @warning Must only be called from thread context.
 * @warning The condition must always be checked in a loop after waking.
 */
void condition_variable_wait(condition_variable_t* condition, mutex_t* mutex);

/**
 * @brief Wake one waiting thread.
 *
 * Removes one thread waiting on the condition variable and makes it runnable.
 *
 * If no thread is waiting, the notification is lost.
 *
 * The awakened thread will return from condition_variable_wait() only after it has
 * successfully reacquired the associated mutex.
 *
 * @param condition Condition variable to signal.
 *
 * @pre condition must be initialized.
 *
 * @note May wake a thread and trigger scheduling activity.
 *
 * @warning Must not be called from interrupt context unless explicitly
 *          supported by the scheduler implementation.
 */
void condition_variable_signal(condition_variable_t* condition);

/**
 * @brief Wake all waiting threads.
 *
 * Makes all threads currently waiting on the condition variable runnable.
 *
 * If no thread is waiting, the notification is lost.
 *
 * All awakened threads will compete to reacquire their associated mutex
 * before returning from condition_wait().
 *
 * @param condition_variable Condition variable to broadcast.
 *
 * @pre condition must be initialized.
 *
 * @note Useful when a state change may satisfy multiple waiting threads.
 *
 * @warning Must not be called from interrupt context unless explicitly
 *          supported by the scheduler implementation.
 */
void condition_variable_broadcast(condition_variable_t* condition_variable);

/** @} */ /* end of condition */
/** @} */ /* end of concurrent */

#ifdef __cplusplus
}
#endif

