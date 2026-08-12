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

    thread_wait_on_queue_context_init(&thread->context, &semaphore->waiters, &semaphore->spinlock);

    scheduler_state_machine_process_event(thread, THREAD_EVENT_BLOCK);
  }
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
    thread_t* waiter = NULL;

    spinlock_lock(&semaphore->spinlock);

    if (!list_is_empty(&semaphore->waiters))
    {
      waiter = CONTAINER_OF(semaphore->waiters.head, thread_t, scheduler_node);
    }
    else
    {
      semaphore->permits++;
    }

    spinlock_unlock(&semaphore->spinlock);

    if (waiter != NULL)
    {
      scheduler_state_machine_process_event(waiter, THREAD_EVENT_WAKEUP);
    }
  }
}

uint32_t semaphore_count(semaphore_t const* semaphore)
{
  return semaphore->permits;
}