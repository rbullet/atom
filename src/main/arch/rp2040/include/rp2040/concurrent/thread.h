#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/thread.h"

static inline void thread_context_wait_queue_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, void* custom_param)
{
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
  context->timeout.timed_out = false;
}

static inline void thread_sleep_context_init(thread_context_t* context, duration_t const timeout)
{
  context->type = THREAD_CONTEXT_SLEEP;
  thread_context_timeout_init(context, timeout);
}

static inline void thread_wait_init(thread_context_t* context)
{
  context->type = THREAD_CONTEXT_WAIT;
}

static inline void thread_wait_with_timeout_context_init(thread_context_t* context, duration_t const timeout)
{
  context->type = THREAD_CONTEXT_WAIT_WITH_TIMEOUT;
  thread_context_timeout_init(context, timeout);
}

static inline void thread_wait_on_queue_context_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock)
{
  context->type = THREAD_CONTEXT_WAIT_ON_QUEUE;
  thread_context_wait_queue_init(context, wait_queue, wait_queue_lock, NULL);
}

static inline void thread_wait_on_queue_with_timeout_context_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, duration_t const timeout)
{
  context->type = THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT;
  thread_context_wait_queue_init(context, wait_queue, wait_queue_lock, NULL);
  thread_context_timeout_init(context, timeout);
}

static inline void thread_wait_on_queue_with_custom_param_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, void* custom_param)
{
  context->type = THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM;
  thread_context_wait_queue_init(context, wait_queue, wait_queue_lock, custom_param);
}

static inline void thread_wait_on_queue_with_custom_param_and_timeout_init(thread_context_t* context, list_t* wait_queue, spinlock_t* wait_queue_lock, void* custom_param, duration_t const timeout)
{
  context->type = THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM_AND_TIMEOUT;
  thread_context_timeout_init(context, timeout);
  thread_context_wait_queue_init(context, wait_queue, wait_queue_lock, custom_param);
}

static inline void thread_terminated_context_init(thread_context_t* context, void* retval)
{
  context->type = THREAD_CONTEXT_TERMINATED;
  context->retval = retval;
}

#ifdef __cplusplus
}
#endif
