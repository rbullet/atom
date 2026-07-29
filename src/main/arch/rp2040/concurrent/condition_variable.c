#include <stdlib.h>
#include "rp2040/config.h"
#include "concurrent/interrupts.h"
#include "concurrent/condition_variable.h"
#include "rp2040/concurrent/spinlock.h"
#include "rp2040/concurrent/scheduler.h"

void condition_variable_wait(condition_variable_t* const condition, mutex_t* const mutex)
{
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(condition_variable_spinlock);
    mutex_unlock(mutex);
    scheduler_thread_block_current_on(&condition->waiters, condition_variable_spinlock);
  }
  mutex_lock(mutex);
}

void condition_variable_signal(condition_variable_t* const condition)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(condition_variable_spinlock)
    {
      if (!list_is_empty(&condition->waiters))
      {
        thread_t* waiter = CONTAINER_OF(list_pop(&condition->waiters), thread_t, scheduler_node); // Pop first waiting thread
        scheduler_thread_resume(waiter);
      }
    }
  }
}

void condition_variable_broadcast(condition_variable_t* condition_variable)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(condition_variable_spinlock)
    {
      while (!list_is_empty(&condition_variable->waiters))
      {
        thread_t* waiter = CONTAINER_OF(list_pop(&condition_variable->waiters), thread_t, scheduler_node);
        scheduler_thread_resume(waiter);
      }
    }
  }
}
