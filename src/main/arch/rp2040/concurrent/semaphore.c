#include <stdlib.h>

#include "rp2040/atom.h"

void semaphore_acquire(semaphore_t* semaphore)
{
  thread_t* const thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&semaphore->spinlock);

    if (semaphore->permits > 0)
    {
      semaphore->permits--;
      spinlock_unlock(&semaphore->spinlock);
      return;
    }

    thread_context_wait_on_queue_init(&thread->context, &semaphore->waiters, &semaphore->spinlock);

    scheduler_state_machine_process_event(thread, THREAD_EVENT_BLOCK);
  }
}

bool semaphore_acquire_with_timeout(semaphore_t* semaphore, duration_t const timeout)
{
  thread_t* const thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&semaphore->spinlock);

    if (semaphore->permits > 0)
    {
      semaphore->permits--;
      spinlock_unlock(&semaphore->spinlock);
      return true;
    }

    thread_context_wait_on_queue_with_timeout_init(&thread->context, &semaphore->waiters, &semaphore->spinlock, timeout);

    scheduler_state_machine_process_event(thread, THREAD_EVENT_BLOCK);
  }

  return thread->context.timeout.wakeup_state == THREAD_WAKEUP_AWOKEN;
}

bool semaphore_try_acquire(semaphore_t* semaphore)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&semaphore->spinlock)
    {
      if (semaphore->permits == 0)
      {
        return false;
      }
      semaphore->permits--;
    }
  }

  return true;
}

void semaphore_release(semaphore_t* semaphore)
{
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&semaphore->spinlock);

    while (!list_is_empty(&semaphore->waiters))
    {
      thread_t* const waiter = CONTAINER_OF(semaphore->waiters.head, thread_t, scheduler_node);

      spinlock_unlock(&semaphore->spinlock);

      bool awoken = false;

      WITH_SPINLOCK(&waiter->state_lock)
      {
        if (waiter->context.timeout.wakeup_state != THREAD_WAKEUP_TIMED_OUT)
        {
          awoken = true;

          if (thread_context_has_timeout(&waiter->context))
          {
            waiter->context.timeout.wakeup_state = THREAD_WAKEUP_AWOKEN;
          }
        }
      }

      spinlock_lock(&semaphore->spinlock);

      if (awoken)
      {
        spinlock_unlock(&semaphore->spinlock);

        scheduler_state_machine_process_event(waiter, THREAD_EVENT_WAKEUP);
        return;
      }
    }

    semaphore->permits++;

    spinlock_unlock(&semaphore->spinlock);
  }
}

uint32_t semaphore_count(semaphore_t const* semaphore)
{
  return semaphore->permits;
}
