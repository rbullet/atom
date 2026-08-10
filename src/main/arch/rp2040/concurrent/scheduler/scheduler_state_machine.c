#include "internal.h"

typedef enum
{
  FAILURE,
  SUCCESS,
  REQUIRES_EXTRA_CONTEXT_SWITCH
} thread_state_transition_result_t;

static thread_state_transition_result_t thread_ready_activate(thread_t* thread);

static void thread_blocked_leave(thread_t* thread);
static thread_state_transition_result_t thread_blocked_activate(thread_t* thread);

static void thread_sleeping_leave(thread_t* thread);
static thread_state_transition_result_t thread_sleeping_activate(thread_t* thread);

static void thread_terminated_leave(thread_t* thread);
static thread_state_transition_result_t thread_terminated_activate(thread_t* thread);

typedef struct
{
  void (*leave)(thread_t*);
  thread_state_transition_result_t (*activate)(thread_t*);
} thread_state_operation_t;

static thread_state_operation_t const thread_states[THREAD_STATE_COUNT] =
{
  [THREAD_NEW] =
  {
    .leave = NULL,
    .activate = NULL,
  },
  [THREAD_READY] =
  {
    .leave = NULL,
    .activate = thread_ready_activate,
  },
  [THREAD_RUNNING] =
  {
    .leave = NULL,
    .activate = NULL,
  },
  [THREAD_BLOCKED] =
  {
    .leave = thread_blocked_leave,
    .activate = thread_blocked_activate,
  },
  [THREAD_SLEEPING] =
  {
    .leave = thread_sleeping_leave,
    .activate = thread_sleeping_activate,
  },
  [THREAD_TERMINATED] =
  {
    .leave = thread_terminated_leave,
    .activate = thread_terminated_activate,
  }
};

inline thread_t* scheduler_thread_current(void)
{
  return execution_context[CPUID].current_thread;
}

static thread_state_t const thread_transitions[THREAD_STATE_COUNT][THREAD_EVENT_COUNT] =
{
  [THREAD_NEW][THREAD_EVENT_START] = THREAD_READY,

  [THREAD_READY][THREAD_EVENT_RUN] = THREAD_RUNNING,

  [THREAD_RUNNING][THREAD_EVENT_YIELD] = THREAD_READY,
  [THREAD_RUNNING][THREAD_EVENT_BLOCK] = THREAD_BLOCKED,
  [THREAD_RUNNING][THREAD_EVENT_SLEEP] = THREAD_SLEEPING,
  [THREAD_RUNNING][THREAD_EVENT_TERMINATE] = THREAD_TERMINATED,

  [THREAD_BLOCKED][THREAD_EVENT_WAKEUP] = THREAD_READY,

  [THREAD_SLEEPING][THREAD_EVENT_WAKEUP] = THREAD_READY,
};

bool scheduler_state_machine_process_event(thread_t* thread, thread_event_t const event)
{
#ifdef DEBUG
  ATOM_ASSERT(thread!=NULL && thread->state_lock.locked==false, "Thread must be non-null and its state lock must be unlocked");
#endif

  thread_state_transition_result_t activate_result = SUCCESS;
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&thread->state_lock)
    {
      thread_state_t const current_state = thread->state;
      thread_state_t const next_state = thread_transitions[current_state][event];
      if (next_state == THREAD_INVALID)
      {
        return false;
      }
      if (thread_states[current_state].leave != NULL)
      {
        thread_states[current_state].leave(thread);
      }
      thread->state = next_state;
      if (thread_states[next_state].activate != NULL)
      {
        activate_result = thread_states[next_state].activate(thread);
      }
    }
  }
  if (activate_result == REQUIRES_EXTRA_CONTEXT_SWITCH && scheduler_thread_current() == thread)
  {
    scheduler_yield();
  }
  return true;
}

static thread_state_transition_result_t thread_ready_activate(thread_t* thread)
{
  thread_t* const current = execution_context[CPUID].current_thread;

  if (thread == execution_context[CPUID].idle_thread)
  {
    return SUCCESS;
  }

  WITH_SPINLOCK(&execution_context[CPUID].spinlock)
  {
    list_push(&execution_context[CPUID].ready_queue, &thread->scheduler_node);
  }

  if (thread == current)
  {
    return REQUIRES_EXTRA_CONTEXT_SWITCH;
  }

  return SUCCESS;
}

static void thread_sleeping_wakeup(void* arg)
{
  thread_t* thread = arg;
  scheduler_state_machine_process_event(thread, THREAD_EVENT_WAKEUP);
}

static thread_state_transition_result_t thread_sleeping_activate(thread_t* thread)
{
#ifdef DEBUG
  ATOM_ASSERT(thread->context.type == THREAD_CONTEXT_SLEEP, "The current thread context must be a THREAD_CONTEXT_SLEEP");
#endif

  thread->context.sleep.wakeup_task.callback = thread_sleeping_wakeup;
  thread->context.sleep.wakeup_task.arg = thread;
  scheduler_deferred_task_start(&thread->context.sleep.wakeup_task);
  return REQUIRES_EXTRA_CONTEXT_SWITCH;
}

static void thread_sleeping_leave(thread_t* thread)
{
#ifdef DEBUG
  ATOM_ASSERT(thread->context.type == THREAD_CONTEXT_SLEEP, "The current thread context must be a THREAD_CONTEXT_SLEEP");
#endif

  scheduler_deferred_task_cancel(&thread->context.sleep.wakeup_task);
}

static void thread_blocked_wakeup(void* arg)
{
  thread_t* thread = arg;
  bool wakeup = false;
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&thread->state_lock)
    {
      if (thread->state == THREAD_BLOCKED)
      {
        thread->context.wait_on_queue_with_timeout.timed_out = true;
        wakeup = true;
      }
    }
  }
  if (wakeup)
  {
    scheduler_state_machine_process_event(thread, THREAD_EVENT_WAKEUP);
  }
}

static thread_state_transition_result_t thread_blocked_activate(thread_t* thread)
{
#ifdef DEBUG
  ATOM_ASSERT(
    (thread->context.type == THREAD_CONTEXT_WAIT) ||
    (thread->context.type == THREAD_CONTEXT_WAIT_ON_QUEUE) ||
    (thread->context.type == THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT) ||
    (thread->context.type == THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM),
    "The current thread context must be a THREAD_CONTEXT_WAIT or THREAD_CONTEXT_WAIT_ON_QUEUE or THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT or THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM"
  );
#endif

  switch (thread->context.type)
  {
  case THREAD_CONTEXT_WAIT:
    break;

  case THREAD_CONTEXT_WAIT_ON_QUEUE:
    list_push(thread->context.wait_on_queue.wait_queue, &thread->scheduler_node);
    spinlock_unlock(thread->context.wait_on_queue.wait_queue_lock);
    break;

  case THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT:
    thread->context.wait_on_queue_with_timeout.wakeup_task.callback = thread_blocked_wakeup;
    thread->context.wait_on_queue_with_timeout.wakeup_task.arg = thread;
    thread->context.wait_on_queue_with_timeout.timed_out = false;
    list_push(thread->context.wait_on_queue_with_timeout.wait_queue, &thread->scheduler_node);
    scheduler_deferred_task_start(&thread->context.wait_on_queue_with_timeout.wakeup_task);
    spinlock_unlock(thread->context.wait_on_queue_with_timeout.wait_queue_lock);
    break;

  case THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM:
    list_push(thread->context.wait_on_queue_with_custom_param.wait_queue, &thread->scheduler_node);
    spinlock_unlock(thread->context.wait_on_queue_with_custom_param.wait_queue_lock);
    break;

  default:
#ifdef DEBUG
    ATOM_ASSERT(false, "Invalid thread context type");
#endif
  }
  return REQUIRES_EXTRA_CONTEXT_SWITCH;
}

static void thread_blocked_leave(thread_t* thread)
{
#ifdef DEBUG
  ATOM_ASSERT(
    (thread->context.type == THREAD_CONTEXT_WAIT) ||
    (thread->context.type == THREAD_CONTEXT_WAIT_ON_QUEUE) ||
    (thread->context.type == THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT) ||
    (thread->context.type == THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM),
    "The current thread context must be a THREAD_CONTEXT_WAIT or THREAD_CONTEXT_WAIT_ON_QUEUE or THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT or THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM"
  );
#endif

  switch (thread->context.type)
  {
  case THREAD_CONTEXT_WAIT:
    break;
  case THREAD_CONTEXT_WAIT_ON_QUEUE:
    WITH_SPINLOCK(thread->context.wait_on_queue.wait_queue_lock)
    {
      if (list_contains(thread->context.wait_on_queue.wait_queue, &thread->scheduler_node))
      {
        list_remove(thread->context.wait_on_queue.wait_queue, &thread->scheduler_node);
      }
    }
    break;
  case THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_TIMEOUT:
    scheduler_deferred_task_cancel(&thread->context.wait_on_queue_with_timeout.wakeup_task);
    WITH_SPINLOCK(thread->context.wait_on_queue_with_timeout.wait_queue_lock)
    {
      if (list_contains(thread->context.wait_on_queue_with_timeout.wait_queue, &thread->scheduler_node))
      {
        list_remove(thread->context.wait_on_queue_with_timeout.wait_queue, &thread->scheduler_node);
      }
    }
    break;
  case THREAD_CONTEXT_WAIT_ON_QUEUE_WITH_CUSTOM_PARAM:
    WITH_SPINLOCK(thread->context.wait_on_queue_with_custom_param.wait_queue_lock)
    {
      if (list_contains(thread->context.wait_on_queue_with_custom_param.wait_queue, &thread->scheduler_node))
      {
        list_remove(thread->context.wait_on_queue_with_custom_param.wait_queue, &thread->scheduler_node);
      }
    }
    break;
  default:
#ifdef DEBUG
    ATOM_ASSERT(false, "Invalid thread context type");
#endif
  }
}

static thread_state_transition_result_t thread_terminated_activate(thread_t* thread)
{
  list_t waiters = LIST_INITIALIZER;

  WITH_SPINLOCK(&thread->waiters_spinlock)
  {
    while (!list_is_empty(&thread->waiters))
    {
      list_push(&waiters, list_pop(&thread->waiters));
    }
  }

  while (!list_is_empty(&waiters))
  {
    thread_t* waiter = CONTAINER_OF(list_pop(&waiters), thread_t, scheduler_node);
    scheduler_state_machine_process_event(waiter, THREAD_EVENT_WAKEUP);
  }
  return REQUIRES_EXTRA_CONTEXT_SWITCH;
}

static void thread_terminated_leave(thread_t* thread)
{
#ifdef DEBUG
  ATOM_ASSERT(false, "Should not leave TERMINATED state");
#endif

  (void)thread;
}
