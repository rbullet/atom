#include <stdlib.h>
#include "rp2040/config.h"
#include "concurrent/mutex.h"
#include "concurrent/interrupts.h"
#include "rp2040/concurrent/spinlock.h"
#include "rp2040/concurrent/scheduler.h"

// --- Lock mutex (block until acquired) ---
void mutex_lock(mutex_t* mutex)
{
  thread_t* thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    for (;;)
    {
      spinlock_lock(mutex_spinlock);

      // Recursive lock
      if (mutex->owner == thread)
      {
        mutex->count++;
        spinlock_unlock(mutex_spinlock);
        return;
      }

      // Acquire unlocked mutex
      if (mutex->owner == NULL)
      {
        mutex->owner = (thread_t*)thread;
        mutex->count = 1;
        spinlock_unlock(mutex_spinlock);
        return;
      }
      // Mutex owned by another thread → block
      scheduler_thread_block_current_on(&mutex->waiters, mutex_spinlock);
    }
  }
}

// --- Try to lock mutex (non-blocking) ---
bool mutex_try_lock(mutex_t* mutex)
{
  thread_t* thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(mutex_spinlock)
    {
      // Recursive lock
      if (mutex->owner == thread)
      {
        mutex->count++;
        return true;
      }
      // Already owned by another thread
      if (mutex->owner != NULL)
      {
        return false;
      }
      // Acquire unlocked mutex
      mutex->owner = (thread_t*)thread;
      mutex->count = 1;
    }
  }
  return true;
}

// --- Unlock mutex ---
void mutex_unlock(mutex_t* mutex)
{
  thread_t* thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(mutex_spinlock)
    {
      // Only the owner may unlock the mutex.
      if (mutex->owner != thread)
      {
        return;
      }
      // Recursive mutex still held
      if (--mutex->count != 0)
      {
        return;
      }
      // Fully release the mutex
      mutex->owner = NULL;
      // Wake one waiting thread, if any
      if (!list_is_empty(&mutex->waiters))
      {
        thread_t* waiter = CONTAINER_OF(list_pop(&mutex->waiters), thread_t, scheduler_node);
        scheduler_thread_resume(waiter);
      }
    }
  }
}