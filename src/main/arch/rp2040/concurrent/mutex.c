#include <stdlib.h>

#include "rp2040/atom.h"

void mutex_lock(mutex_t* mutex)
{
  thread_t* const thread = thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&mutex->spinlock);
    if (mutex->owner == thread)
    {
      mutex->count++;
      spinlock_unlock(&mutex->spinlock);
      return;
    }
    if (mutex->owner == NULL)
    {
      mutex->owner = thread;
      mutex->count = 1;
      spinlock_unlock(&mutex->spinlock);
      return;
    }
    thread_context_wait_on_queue_init(&thread->context, &mutex->waiters, &mutex->spinlock);
    scheduler_state_machine_process_event(thread, THREAD_EVENT_BLOCK);
  }
}

bool mutex_try_lock(mutex_t* mutex)
{
  thread_t* const thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&mutex->spinlock)
    {
      if (mutex->owner == thread)
      {
        mutex->count++;
        return true;
      }
      if (mutex->owner != NULL)
      {
        return false;
      }
      mutex->owner = thread;
      mutex->count = 1;
    }
  }
  return true;
}

void mutex_unlock(mutex_t* mutex)
{
  thread_t const* const thread = thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&mutex->spinlock);

    if (mutex->owner != thread)
    {
      spinlock_unlock(&mutex->spinlock);
      return;
    }

    if (--mutex->count != 0)
    {
      spinlock_unlock(&mutex->spinlock);
      return;
    }

    thread_t* waiter = NULL;
    if (!list_is_empty(&mutex->waiters))
    {
      waiter = CONTAINER_OF(mutex->waiters.head, thread_t, scheduler_node);
      mutex->owner = waiter;
      mutex->count = 1;
    }
    else
    {
      mutex->owner = NULL;
    }

    spinlock_unlock(&mutex->spinlock);

    if (waiter != NULL)
    {
      scheduler_state_machine_process_event(waiter, THREAD_EVENT_WAKEUP);
    }
  }
}
