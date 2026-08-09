#pragma once
#include "deferred_task.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "concurrent/spinlock.h"
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
  THREAD_NEW,
  THREAD_READY,       ///< Thread is ready to execute.
  THREAD_RUNNING,     ///< Thread is currently executing.
  THREAD_BLOCKED,     ///< Thread is waiting for an event.
  THREAD_SLEEPING,    ///< Thread is waiting for a timeout.
  THREAD_TERMINATED,  ///< Thread execution has completed.
  /**
   * @cond INTERNAL
   */
  THREAD_STATE_COUNT
  /**
   *@endcond
   */
} thread_state_t;

typedef enum
{
  THREAD_CONTEXT_NONE = 0,
  THREAD_CONTEXT_SLEEP,
  THREAD_CONTEXT_WAIT,
  THREAD_CONTEXT_WAIT_ON_QUEUE,
  THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT,
  THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM,
  THREAD_CONTEXT_TERMINATED,
} thread_context_type_t;

typedef struct
{
  thread_context_type_t type;
} thread_context_t;

typedef struct
{
  thread_context_t base;     // MUST BE THREAD_CONTEXT_SLEEP
  deferred_task_t wakeup_task;
} thread_sleep_context_t;

static inline void thread_sleep_context_init(thread_sleep_context_t* context, duration_t const timeout)
{
  context->base.type = THREAD_CONTEXT_SLEEP;
  context->wakeup_task.state = DEFERRED_TASK_IDLE;
  context->wakeup_task.mutex = MUTEX_INITIALIZER;
  context->wakeup_task.completion = CONDITION_VARIABLE_INITIALIZER;
  context->wakeup_task.initial_delay = timeout;
  context->wakeup_task.period = duration_of(0, MILLISECONDS);
}

typedef struct
{
  thread_context_t base;     // MUST BE THREAD_CONTEXT_WAIT
} thread_wait_context_t;

static inline void thread_wait_init(thread_wait_context_t* context)
{
  context->base.type = THREAD_CONTEXT_WAIT;
}

typedef struct
{
  thread_context_t base;     // MUST BE THREAD_CONTEXT_WAIT_ON_QUEUE
  list_t* wait_queue;
  spinlock_t* wait_queue_lock;
} thread_wait_on_queue_context_t;

static inline void thread_wait_on_queue_context_init(thread_wait_on_queue_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock)
{
  context->base.type = THREAD_CONTEXT_WAIT_ON_QUEUE;
  context->wait_queue = wait_queue;
  context->wait_queue_lock = wait_queue_lock;
}

#define THREAD_WAIT_ON_QUEUE_INITIALIZER = ((thread_wait_on_queue_context_t){ .base = { .type = THREAD_CONTEXT_WAIT_ON_QUEUE } })

typedef struct
{
  thread_context_t base;     // MUST BE THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT
  list_t* wait_queue;
  spinlock_t* wait_queue_lock;
  deferred_task_t wakeup_task;
  bool timed_out;
} thread_wait_on_queue_with_timeout_context_t;

static inline void thread_wait_on_queue_with_timeout_context_init(thread_wait_on_queue_with_timeout_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, duration_t const timeout)
{
  context->base.type = THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT;
  context->wait_queue = wait_queue;
  context->wait_queue_lock = wait_queue_lock;
  context->timed_out = false;
  context->wakeup_task.state = DEFERRED_TASK_IDLE;
  context->wakeup_task.mutex = MUTEX_INITIALIZER;
  context->wakeup_task.completion = CONDITION_VARIABLE_INITIALIZER;
  context->wakeup_task.initial_delay = timeout;
  context->wakeup_task.period = duration_of(0, MILLISECONDS);
}

typedef struct
{
  thread_context_t base;     // MUST BE THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM
  list_t* wait_queue;
  spinlock_t* wait_queue_lock;
  void* custom_param;
} thread_wait_on_queue_with_custom_param_context_t;

static inline void thread_wait_on_queue_with_custom_param_init(thread_wait_on_queue_with_custom_param_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, void* custom_param)
{
  context->base.type = THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM;
  context->wait_queue = wait_queue;
  context->wait_queue_lock = wait_queue_lock;
  context->custom_param = custom_param;
}

typedef struct
{
  thread_context_t base;    // THREAD_CONTEXT_TERMINATED
  void* retval;
} thread_terminated_context_t;

static inline void thread_terminated_context_init(thread_terminated_context_t* context, void* retval)
{
  context->base.type = THREAD_CONTEXT_TERMINATED;
  context->retval = retval;
}

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
  spinlock_t state_lock;            // Spinlock for protecting thread state.
  list_t waiters;                   // Threads waiting for termination.
  spinlock_t waiters_spinlock;      // Spinlock for protecting waiters list.
  union
  {
    thread_context_type_t type;
    thread_sleep_context_t sleep;
    thread_wait_context_t wait;
    thread_wait_on_queue_context_t wait_on_queue;
    thread_wait_on_queue_with_timeout_context_t wait_on_queue_with_timeout;
    thread_wait_on_queue_with_custom_param_context_t wait_on_queue_with_custom_param;
    thread_terminated_context_t terminated;
  } context;
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