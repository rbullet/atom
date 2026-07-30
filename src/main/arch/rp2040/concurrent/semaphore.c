#include <stdlib.h>

#include <atom.h>
#include "rp2040/concurrent/spinlock.h"
#include "rp2040/concurrent/scheduler.h"

#define SEMAPHORE_SPINLOCK spinlock26

void semaphore_acquire(semaphore_t* semaphore)
{
  WITH_INTERRUPTS_DISABLED
  {
    for (;;)
    {
      spinlock_lock(SEMAPHORE_SPINLOCK);
      if (semaphore->permits > 0)
      {
        semaphore->permits--;
        spinlock_unlock(SEMAPHORE_SPINLOCK);
        return;
      }
      scheduler_thread_block_current_on(&semaphore->waiters, SEMAPHORE_SPINLOCK);
    }
  }
}

bool semaphore_try_acquire(semaphore_t* semaphore)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(SEMAPHORE_SPINLOCK)
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
    WITH_SPINLOCK(SEMAPHORE_SPINLOCK)
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

#undef SEMAPHORE_SPINLOCK
