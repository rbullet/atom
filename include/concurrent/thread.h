#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "concurrent/mutex.h"
#include "concurrent/condition_variable.h"
#include "util/collection/list.h"
#include "util/time/duration.h"
#include "util/time/timestamp.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup thread Threads
 * @brief Lightweight preemptive execution contexts with explicit synchronization support.
 *
 * Provides stack-based preemptive threads with explicit scheduling,
 * sleeping, notification and joining.
 *
 * Thread objects and stacks are supplied by the application.
 * No dynamic allocation is performed by the thread subsystem.
 *
 * @ingroup concurrent
 * @{
 */

/**
 * @brief Thread execution state.
 *
 * This state is managed by the scheduler.
 * Applications must not modify it directly.
 */
typedef enum
{
  THREAD_INVALID = 0, ///< Thread has not been initialized.
  THREAD_READY,       ///< Thread is ready to execute.
  THREAD_RUNNING,     ///< Thread is currently executing.
  THREAD_BLOCKED,     ///< Thread is waiting for an event.
  THREAD_SLEEPING,    ///< Thread is waiting for a timeout.
  THREAD_TERMINATED,  ///< Thread execution has completed.

} thread_state_t;


/**
 * @brief Thread control block.
 *
 * Represents the execution context and scheduler state of a thread.
 *
 * Thread objects are allocated by the application and must remain valid
 * for the lifetime of the thread.
 *
 * Applications should not modify fields directly.
 */
typedef struct thread_t
{
  uint32_t* sp;                     // Saved stack pointer.
  uint32_t* stack_base;             // Stack memory base address.
  size_t stack_size;                // Stack size in bytes.
  timestamp_t deadline;             // Scheduler ticks or timeout counter.
  thread_state_t state;             // Current execution state.
  list_node_t scheduler_node;       // Scheduler queue node.
  list_t waiters;                   // Threads waiting for termination.
  mutex_t mutex;
  condition_variable_t completion;
  void* wait_param;
  void* retval;                     // Thread return value.
} thread_t;


/**
 * @brief Thread entry function.
 *
 * Returning from this function terminates the thread.
 * The returned value can be retrieved with thread_join().
 *
 * @param arg User supplied argument.
 *
 * @return Thread termination value.
 */
typedef void* (*thread_func_t)(void* arg);

/**
 * @brief Initialize a thread.
 *
 * Initializes the thread object and creates its initial CPU context.
 *
 * No memory allocation is performed.
 * The caller owns the thread object and its stack.
 *
 * @param thread Thread object.
 * @param stack_base Stack memory base address.
 * @param stack_size Stack size in bytes.
 * @param start_routine Thread entry function.
 * @param arg Argument passed to the entry function.
 *
 * @pre thread must point to valid storage.
 * @pre stack_base must point to writable memory.
 *
 * @warning Must not be called from interrupt context.
 */
void thread_init(thread_t* thread, uint32_t* stack_base, size_t stack_size, thread_func_t start_routine, void* arg);

/**
 * @brief Add a thread to the scheduler.
 *
 * Makes an initialized thread eligible for execution.
 *
 * @param thread Thread to start.
 *
 * @pre thread must have been initialized with thread_init().
 *
 * @warning Starting a thread more than once is undefined behavior.
 */
void thread_start(thread_t* thread);

/**
 * @brief Yield execution voluntarily.
 *
 * The current thread remains runnable and allows another thread to execute.
 *
 * @note Must be called from thread context.
 */
void thread_yield(void);

/**
 * @brief Sleep for a duration.
 *
 * The current thread becomes sleeping and is automatically resumed after
 * the timeout expires.
 *
 * @param duration Sleep duration.
 *
 * @note Must be called from thread context.
 */
void thread_sleep(duration_t duration);

/**
 * @brief Get the current running thread.
 *
 * @return Current thread object.
 *
 * @note Safe from thread and interrupt context.
 */
thread_t* thread_current(void);

/**
 * @brief Wait for a notification.
 *
 * Blocks the current thread until another thread calls thread_notify().
 *
 * Notifications are not stored. A notification sent before waiting is lost.
 *
 * @note Must be called from thread context.
 */
void thread_wait(void);

/**
 * @brief Wake a waiting thread.
 *
 * Makes a thread blocked in thread_wait() runnable.
 *
 * @param thread Thread to notify.
 *
 * @note Safe from interrupt context.
 */
void thread_notify(thread_t* thread);

/**
 * @brief Wait for thread completion.
 *
 * Blocks until the specified thread terminates.
 *
 * @param thread Thread to join.
 *
 * @return Thread return value.
 *
 * @pre thread must not be the current thread.
 *
 * @note Must be called from thread context.
 */
void* thread_join(thread_t* thread);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif