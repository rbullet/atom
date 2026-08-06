#include "rp2040/atom.h"

void condition_variable_wait(condition_variable_t* const condition_variable, mutex_t* const mutex)
{
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&condition_variable->spinlock);
    mutex_unlock(mutex);
    scheduler_thread_block_current_on(&condition_variable->waiters, &condition_variable->spinlock);
  }
  mutex_lock(mutex);
}

void condition_variable_signal(condition_variable_t* const condition_variable)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&condition_variable->spinlock)
    {
      if (!list_is_empty(&condition_variable->waiters))
      {
        thread_t* const waiter = CONTAINER_OF(list_pop(&condition_variable->waiters), thread_t, scheduler_node); // Pop first waiting thread
        scheduler_thread_resume(waiter);
      }
    }
  }
}

void condition_variable_broadcast(condition_variable_t* condition_variable)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&condition_variable->spinlock)
    {
      while (!list_is_empty(&condition_variable->waiters))
      {
        thread_t* const waiter = CONTAINER_OF(list_pop(&condition_variable->waiters), thread_t, scheduler_node);
        scheduler_thread_resume(waiter);
      }
    }
  }
}

