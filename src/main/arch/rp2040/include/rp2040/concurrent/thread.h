#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/thread.h"

static inline void thread_context_init(thread_context_t* context)
{
  context->wait.wait_queue = NULL;
  context->wait.wait_queue_lock = NULL;
  context->wait.custom_param = NULL;
  context->timeout.wakeup_state = THREAD_WAKEUP_NONE;
}

static inline void thread_context_wait_queue_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, void* custom_param)
{
#ifdef DEBUG
  ATOM_ASSERT(wait_queue != NULL && wait_queue_lock != NULL,"Wait queue and lock must not be NULL");
#endif
  context->wait.wait_queue = wait_queue;
  context->wait.wait_queue_lock = wait_queue_lock;
  context->wait.custom_param = custom_param;
}

static inline void thread_context_timeout_init(thread_context_t* context, duration_t const timeout_duration)
{
  context->timeout.wakeup_task.state = DEFERRED_TASK_IDLE;
  context->timeout.wakeup_task.mutex = MUTEX_INITIALIZER;
  context->timeout.wakeup_task.completion = CONDITION_VARIABLE_INITIALIZER;
  context->timeout.wakeup_task.initial_delay = timeout_duration;
  context->timeout.wakeup_task.period = duration_of(0, MILLISECONDS);
  context->timeout.wakeup_state = THREAD_WAKEUP_PENDING;
}

static inline void thread_context_sleep_init(thread_context_t* context, duration_t const timeout)
{
  thread_context_init(context);
  thread_context_timeout_init(context, timeout);
}

static inline void thread_context_wait_init(thread_context_t* context)
{
  thread_context_init(context);
}

static inline void thread_context_wait_with_timeout_init(thread_context_t* context, duration_t const timeout)
{
  thread_context_init(context);
  thread_context_timeout_init(context, timeout);
}

static inline void thread_context_wait_on_queue_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock)
{
  thread_context_init(context);
  thread_context_wait_queue_init(context, wait_queue, wait_queue_lock, NULL);
}

static inline void thread_context_wait_on_queue_with_timeout_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, duration_t const timeout)
{
  thread_context_init(context);
  thread_context_wait_queue_init(context, wait_queue, wait_queue_lock, NULL);
  thread_context_timeout_init(context, timeout);
}

static inline void thread_context_wait_on_queue_with_custom_param_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, void* custom_param)
{
  thread_context_init(context);
  thread_context_wait_queue_init(context, wait_queue, wait_queue_lock, custom_param);
}

static inline void thread_context_wait_on_queue_with_custom_param_and_timeout_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, void* custom_param, duration_t const timeout)
{
  thread_context_init(context);
  thread_context_timeout_init(context, timeout);
  thread_context_wait_queue_init(context, wait_queue, wait_queue_lock, custom_param);
}

static inline void thread_context_terminated_init(thread_context_t* context, void* retval)
{
  thread_context_init(context);
  context->retval = retval;
}

static inline bool thread_context_has_timeout(thread_context_t const* context)
{
  return context->timeout.wakeup_state != THREAD_WAKEUP_NONE;
}

static inline bool thread_context_has_queue(thread_context_t const* context)
{
  return context->wait.wait_queue != NULL;
}

#ifdef __cplusplus
}
#endif
