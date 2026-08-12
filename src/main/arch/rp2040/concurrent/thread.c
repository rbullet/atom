#include "rp2040/atom.h"

void thread_init(thread_t* thread, uint32_t* stack_base, size_t const stack_size, thread_func_t const start_routine, void* arg)
{
  scheduler_thread_init(thread, stack_base, stack_size, start_routine, arg);
}

void thread_start(thread_t* thread)
{
  scheduler_state_machine_process_event(thread, THREAD_EVENT_START);
}

thread_t* thread_current(void)
{
  return scheduler_thread_current();
}

void thread_yield(void)
{
  scheduler_state_machine_process_event(thread_current(), THREAD_EVENT_YIELD);
}

void thread_wait(void)
{
  thread_t* thread = thread_current();
  thread_context_wait_init(&thread->context);
  scheduler_state_machine_process_event(thread, THREAD_EVENT_BLOCK);
}

bool thread_wait_with_timeout(duration_t const timeout)
{
  thread_t* thread = thread_current();
  thread_context_wait_with_timeout_init(&thread->context, timeout);
  scheduler_state_machine_process_event(thread, THREAD_EVENT_BLOCK);
  return !thread->context.timeout.timed_out;
}

void thread_notify(thread_t* thread)
{
  scheduler_state_machine_process_event(thread, THREAD_EVENT_WAKEUP);
}

void thread_sleep(duration_t const duration)
{
  thread_t* thread = thread_current();
  thread_context_sleep_init(&thread->context, duration);
  scheduler_state_machine_process_event(thread, THREAD_EVENT_SLEEP);
}

void thread_join(thread_t* thread, void** retval)
{
  thread_t* const current = thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&thread->state_lock);
    if (thread->state == THREAD_TERMINATED)
    {
      spinlock_unlock(&thread->state_lock);
    }
    else
    {
      spinlock_lock(&thread->waiters_spinlock);
      thread_context_wait_on_queue_init(&current->context, &thread->waiters, &thread->waiters_spinlock);
      spinlock_unlock(&thread->state_lock);
      scheduler_state_machine_process_event(current, THREAD_EVENT_BLOCK);
    }
  }
  if (retval)
  {
    *retval = thread->context.retval;
  }
}

bool thread_join_with_timeout(thread_t* thread, void** retval, duration_t const timeout)
{
  thread_t* const current = thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&thread->state_lock);
    if (thread->state == THREAD_TERMINATED)
    {
      spinlock_unlock(&thread->state_lock);
      if (retval)
      {
        *retval = thread->context.retval;
      }
      return true;
    }

    spinlock_lock(&thread->waiters_spinlock);
    thread_context_wait_on_queue_with_timeout_init(&current->context, &thread->waiters, &thread->waiters_spinlock, timeout);
    spinlock_unlock(&thread->state_lock);
    scheduler_state_machine_process_event(current, THREAD_EVENT_BLOCK);
  }
  bool const timed_out = current->context.timeout.timed_out;
  if (!timed_out && retval)
  {
    *retval = thread->context.retval;
  }
  return !timed_out;
}
