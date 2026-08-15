#include <stdlib.h>

#include "rp2040/atom.h"

static bool mutex_lock_internal(mutex_t* mutex, duration_t const* const timeout)
{
  thread_t* const thread = thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&mutex->spinlock);
    if (mutex->owner == thread)
    {
      mutex->count++;
      spinlock_unlock(&mutex->spinlock);
      return true;
    }
    if (mutex->owner == NULL)
    {
      mutex->owner = thread;
      mutex->count = 1;
      spinlock_unlock(&mutex->spinlock);
      return true;
    }

    if (timeout == NULL)
    {
      thread_context_wait_on_queue_init(&thread->context, &mutex->waiters, &mutex->spinlock);
    }
    else
    {
      thread_context_wait_on_queue_with_timeout_init(&thread->context, &mutex->waiters, &mutex->spinlock, *timeout);
    }

    scheduler_state_machine_process_event(thread, THREAD_EVENT_BLOCK);
  }

  return (timeout == NULL) || (thread->context.timeout.wakeup_state == THREAD_WAKEUP_AWOKEN);
}

void mutex_lock(mutex_t* mutex)
{
  mutex_lock_internal(mutex, NULL);
}

bool mutex_lock_with_timeout(mutex_t* mutex, duration_t timeout)
{
  return mutex_lock_internal(mutex, &timeout);
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

    while (!list_is_empty(&mutex->waiters))
    {
      thread_t* const waiter = CONTAINER_OF(mutex->waiters.head, thread_t, scheduler_node);

      spinlock_unlock(&mutex->spinlock);

      bool awoken = false;

      WITH_SPINLOCK(&waiter->state_lock)
      {
        if (waiter->context.timeout.wakeup_state != THREAD_WAKEUP_TIMED_OUT && waiter->context.timeout.wakeup_state != THREAD_WAKEUP_AWOKEN)
        {
          awoken = true;
          waiter->context.timeout.wakeup_state = THREAD_WAKEUP_AWOKEN;
        }
      }

      spinlock_lock(&mutex->spinlock);

      if (awoken)
      {
        mutex->owner = waiter;
        mutex->count = 1;

        spinlock_unlock(&mutex->spinlock);

        scheduler_state_machine_process_event(waiter, THREAD_EVENT_WAKEUP);
        return;
      }
    }

    mutex->owner = NULL;

    spinlock_unlock(&mutex->spinlock);
  }
}
