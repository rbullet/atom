#include <stdlib.h>

#include <atom.h>
#include "rp2040/concurrent/spinlock.h"
#include "rp2040/concurrent/scheduler.h"

#define MUTEX_SPINLOCK spinlock27

void mutex_lock(mutex_t* mutex)
{
  thread_t* const thread = thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    for (;;)
    {
      spinlock_lock(MUTEX_SPINLOCK);

      if (mutex->owner == thread)
      {
        mutex->count++;
        spinlock_unlock(MUTEX_SPINLOCK);
        return;
      }

      if (mutex->owner == NULL)
      {
        mutex->owner = thread;
        mutex->count = 1;
        spinlock_unlock(MUTEX_SPINLOCK);
        return;
      }
      scheduler_thread_block_current_on(&mutex->waiters, MUTEX_SPINLOCK);
    }
  }
}

bool mutex_try_lock(mutex_t* mutex)
{
  thread_t* const thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(MUTEX_SPINLOCK)
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
  thread_t* const thread = thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(MUTEX_SPINLOCK)
    {
      if (mutex->owner != thread)
      {
        return;
      }
      if (--mutex->count != 0)
      {
        return;
      }
      mutex->owner = NULL;
      if (!list_is_empty(&mutex->waiters))
      {
        thread_t* const waiter = CONTAINER_OF(list_pop(&mutex->waiters), thread_t, scheduler_node);
        scheduler_thread_resume(waiter);
      }
    }
  }
}

#undef MUTEX_SPINLOCK
