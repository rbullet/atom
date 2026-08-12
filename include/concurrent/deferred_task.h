#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "util/collection/list.h"
#include "util/time/duration.h"
#include "util/time/timestamp.h"
#include "concurrent/mutex.h"
#include "concurrent/condition_variable.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup concurrent_deferred_task Deferred tasks
 * @brief A mechanism for scheduling delayed or asynchronous work outside interrupt context.
 *
 * Deferred tasks allow scheduling callbacks to execute after a delay or
 * periodically after an initial delay.
 *
 * A deferred task is owned by the scheduler while it is active. The caller
 * must ensure that the deferred_task_t storage remains valid until the task
 * reaches a completed or cancelled state.
 *
 * @{
 */

/**
 * @brief Callback executed when a deferred task expires.
 *
 * The callback executes in the deferred task manager thread context.
 *
 * @param arg User-provided argument passed during task creation.
 */
typedef void (*deferred_task_callback_t)(void* arg);

/** @cond INTERNAL */

/**
 * @brief Deferred task lifecycle state.
 */
typedef enum
{
  DEFERRED_TASK_IDLE,  ///< Task object initialized but not scheduled.
  DEFERRED_TASK_SCHEDULED, ///< Task is waiting for its deadline.
  DEFERRED_TASK_RUNNING,  ///< Task callback is currently executing.
  DEFERRED_TASK_COMPLETED,  ///< One-shot task completed successfully.
  DEFERRED_TASK_CANCELLED  ///< Task was cancelled. This is a terminal state.
} deferred_task_state_t;

/** @endcond */

/**
 * @brief Deferred task object.
 *
 * A deferred_task_t instance must not be copied while active.
 *
 * The caller must not restart or reschedule an active task. Starting an
 * already scheduled or running task is considered a programming error.
 */
typedef struct deferred_task_t
{
  /** @cond INTERNAL */
  duration_t initial_delay; ///< Delay before the first execution.
  duration_t period; ///< Period between executions.
  deferred_task_callback_t callback; ///< Callback executed when the task expires.
  void* arg; ///< User argument passed to the callback.
  deferred_task_state_t state; ///< Current task lifecycle state.
  list_node_t scheduler_node; ///< Internal scheduler list node.
  timestamp_t deadline; ///< Next execution deadline.
  mutex_t mutex; ///< Internal synchronization mutex.
  condition_variable_t completion; ///< Used to wait for task completion or cancellation.
  /** @endcond */
} deferred_task_t;

/**
 * @brief Starts a one-shot deferred task.
 *
 * The callback is executed once after @p initial_delay.
 *
 * After execution, the task enters the DEFERRED_TASK_COMPLETED state.
 *
 * The deferred_task_t object must not already be active.
 *
 * @param deferred_task Deferred task object.
 * @param initial_delay Delay before callback execution.
 * @param callback Callback to execute.
 * @param arg Argument passed to the callback.
 */
void deferred_task_start_after(deferred_task_t* deferred_task, duration_t initial_delay, deferred_task_callback_t callback, void* arg);

/**
 * @brief Starts a periodic deferred task.
 *
 * The first callback execution occurs after @p initial_delay.
 * Subsequent executions occur every @p period after completion of the
 * previous callback.
 *
 * The deferred_task_t object must not already be active.
 *
 * The task remains active until cancelled.
 *
 * @param deferred_task Deferred task object.
 * @param initial_delay Delay before the first execution.
 * @param period Period between executions.
 * @param callback Callback to execute periodically.
 * @param arg Argument passed to the callback.
 */
void deferred_task_start_periodic(deferred_task_t* deferred_task, duration_t initial_delay, duration_t period, deferred_task_callback_t callback, void* arg);

/**
 * @brief Checks whether a deferred task is currently active.
 *
 * A task is considered active while it is either scheduled or executing.
 *
 * This function only returns a snapshot of the current state. The result may
 * become invalid immediately after returning if another thread changes the task
 * state.
 *
 * @param deferred_task Deferred task object.
 *
 * @return true if the task is scheduled or running, false otherwise.
 */
static inline bool deferred_task_is_active(deferred_task_t const* deferred_task)
{
  return (deferred_task->state == DEFERRED_TASK_SCHEDULED) || (deferred_task->state == DEFERRED_TASK_RUNNING);
}

/**
 * @brief Cancels a deferred task.
 *
 * Cancellation is a terminal operation. A cancelled task will never be
 * scheduled again.
 *
 * If the task callback is currently executing from another thread, this
 * function waits until the callback has returned before completing.
 *
 * If called from inside the task callback itself, the cancellation is marked
 * but the function does not wait for completion.
 *
 * @param deferred_task Deferred task object.
 */
void deferred_task_cancel(deferred_task_t* deferred_task);

/** @} */ /* end of concurrent_deferred_task */
/** @} */ /* end of concurrent */

#ifdef __cplusplus
}
#endif