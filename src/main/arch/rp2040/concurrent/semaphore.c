#include <stdlib.h>

#include "concurrent/interrupts.h"
#include "concurrent/semaphore.h"
#include "rp2040/config.h"
#include "rp2040/concurrent/spinlock.h"
#include "rp2040/concurrent/scheduler.h"

void semaphore_acquire(semaphore_t* semaphore)
{
  WITH_INTERRUPTS_DISABLED
  {
    for (;;)
    {
      spinlock_lock(semaphore_spinlock);
      if (semaphore->permits > 0)
      {
        semaphore->permits--;
        spinlock_unlock(semaphore_spinlock);
        return;
      }
      scheduler_thread_block_current_on(&semaphore->waiters, semaphore_spinlock);
    }
  }
}

bool semaphore_try_acquire(semaphore_t* semaphore)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(semaphore_spinlock)
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
    WITH_SPINLOCK(semaphore_spinlock)
    {
      semaphore->permits++;
      if (!list_is_empty(&semaphore->waiters))
      {
        thread_t* waiter = CONTAINER_OF(list_pop(&semaphore->waiters), thread_t, scheduler_node);
        scheduler_thread_resume(waiter);
      }
    }
  }
}

uint32_t semaphore_count(semaphore_t const* semaphore)
{
  return semaphore->permits;
}
