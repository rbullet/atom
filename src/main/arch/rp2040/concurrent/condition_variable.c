#include "rp2040/atom.h"
#include "scheduler/internal.h"

void condition_variable_wait(condition_variable_t* const condition_variable, mutex_t* const mutex)
{
  thread_t* const thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&condition_variable->spinlock);

    mutex_unlock(mutex);

    thread_wait_on_queue_context_init(&thread->context.wait_on_queue, &condition_variable->waiters, &condition_variable->spinlock);

    thread_process_event(thread, THREAD_EVENT_BLOCK);
  }

  mutex_lock(mutex);
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

    if (waiter != NULL)
    {
      thread_process_event(waiter, THREAD_EVENT_WAKEUP);
    }
  }
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

    while (!list_is_empty(&resume_list))
    {
      thread_t* const waiter = CONTAINER_OF(list_pop(&resume_list), thread_t, scheduler_node);
      thread_process_event(waiter, THREAD_EVENT_WAKEUP);
    }
  }
}
