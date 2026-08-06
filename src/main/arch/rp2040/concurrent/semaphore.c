#include <stdlib.h>

#include <atom.h>
#include "rp2040/concurrent/scheduler.h"

void semaphore_acquire(semaphore_t* semaphore)
{
  WITH_INTERRUPTS_DISABLED
  {
    for (;;)
    {
      spinlock_lock(&semaphore->spinlock);
      if (semaphore->permits > 0)
      {
        semaphore->permits--;
        spinlock_unlock(&semaphore->spinlock);
        return;
      }
      scheduler_thread_block_current_on(&semaphore->waiters, &semaphore->spinlock);
    }
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
    WITH_SPINLOCK(&semaphore->spinlock)
    {
      semaphore->permits++;
      if (!list_is_empty(&semaphore->waiters))
      {
        thread_t* const waiter = CONTAINER_OF(list_pop(&semaphore->waiters), thread_t, scheduler_node);
        scheduler_thread_resume(waiter);
      }
    }
  }
}

uint32_t semaphore_count(semaphore_t const* semaphore)
{
  return semaphore->permits;
}
