#include "rp2040/atom.h"

#define CONDITION_VARIABLE_SPINLOCK spinlock25

void condition_variable_wait(condition_variable_t* const condition, mutex_t* const mutex)
{
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(CONDITION_VARIABLE_SPINLOCK);
    mutex_unlock(mutex);
    scheduler_thread_block_current_on(&condition->waiters, CONDITION_VARIABLE_SPINLOCK);
  }
  mutex_lock(mutex);
}

void condition_variable_signal(condition_variable_t* const condition)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(CONDITION_VARIABLE_SPINLOCK)
    {
      if (!list_is_empty(&condition->waiters))
      {
        thread_t* const waiter = CONTAINER_OF(list_pop(&condition->waiters), thread_t, scheduler_node); // Pop first waiting thread
        scheduler_thread_resume(waiter);
      }
    }
  }
}

void condition_variable_broadcast(condition_variable_t* condition_variable)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(CONDITION_VARIABLE_SPINLOCK)
    {
      while (!list_is_empty(&condition_variable->waiters))
      {
        thread_t* const waiter = CONTAINER_OF(list_pop(&condition_variable->waiters), thread_t, scheduler_node);
        scheduler_thread_resume(waiter);
      }
    }
  }
}

#undef CONDITION_VARIABLE_SPINLOCK
