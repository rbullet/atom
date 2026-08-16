#include "rp2040/atom.h"

static bool condition_variable_wait_internal(condition_variable_t* const condition_variable, mutex_t* const mutex, duration_t const* const timeout)
{
  thread_t* const current = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&condition_variable->spinlock);

    mutex_unlock(mutex);

    if (timeout == NULL)
    {
      thread_context_wait_on_queue_init(&current->context, &condition_variable->waiters, &condition_variable->spinlock);
    }
    else
    {
      thread_context_wait_on_queue_with_timeout_init(&current->context, &condition_variable->waiters, &condition_variable->spinlock, *timeout);
    }

    scheduler_state_machine_process_event(current, THREAD_EVENT_BLOCK);
  }
  WITH_INTERRUPTS_DISABLED_END

  mutex_lock(mutex);

  return (timeout == NULL) || (current->context.timeout.wakeup_state == THREAD_WAKEUP_AWOKEN);
}

void condition_variable_wait(condition_variable_t* const condition_variable, mutex_t* const mutex)
{
  condition_variable_wait_internal(condition_variable, mutex, NULL);
}

bool condition_variable_wait_with_timeout(condition_variable_t* const condition_variable, mutex_t* const mutex, duration_t const timeout)
{
  return condition_variable_wait_internal(condition_variable, mutex, &timeout);
}

void condition_variable_signal(condition_variable_t* const condition_variable)
{
  thread_t* waiter = NULL;

  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&condition_variable->spinlock)
    {
      if (!list_is_empty(&condition_variable->waiters))
      {
        waiter = CONTAINER_OF(condition_variable->waiters.head, thread_t, scheduler_node);
      }
    }
    WITH_SPINLOCK_END

    if (waiter != NULL)
    {
      scheduler_state_machine_process_event(waiter, THREAD_EVENT_WAKEUP);
    }
  }
  WITH_INTERRUPTS_DISABLED_END
}

void condition_variable_broadcast(condition_variable_t* condition_variable)
{
  list_t resume_list = LIST_INITIALIZER;

  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&condition_variable->spinlock)
    {
      while (!list_is_empty(&condition_variable->waiters))
      {
        list_push(&resume_list, list_pop(&condition_variable->waiters));
      }
    }
    WITH_SPINLOCK_END

    while (!list_is_empty(&resume_list))
    {
      thread_t* const waiter = CONTAINER_OF(list_pop(&resume_list), thread_t, scheduler_node);
      scheduler_state_machine_process_event(waiter, THREAD_EVENT_WAKEUP);
    }
  }
  WITH_INTERRUPTS_DISABLED_END
}
