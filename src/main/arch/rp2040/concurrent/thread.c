#include "rp2040/concurrent/scheduler.h"
#include "scheduler/internal.h"

void thread_init(thread_t* thread, uint32_t* stack_base, size_t const stack_size, thread_func_t const start_routine, void* arg)
{
  scheduler_thread_init(thread, stack_base, stack_size, start_routine, arg);
}

void thread_start(thread_t* thread)
{
  thread_process_event(thread, THREAD_EVENT_START);
}

thread_t* thread_current(void)
{
  return scheduler_thread_current();
}

void thread_yield(void)
{
  thread_process_event(thread_current(), THREAD_EVENT_YIELD);
}

void thread_wait(void)
{
  thread_t* thread = thread_current();
  thread_wait_init(&thread->context.wait);
  thread_process_event(thread, THREAD_EVENT_BLOCK);
}

void thread_notify(thread_t* thread)
{
  thread_process_event(thread, THREAD_EVENT_WAKEUP);
}

void thread_sleep(duration_t const duration)
{
  thread_t* thread = thread_current();
  thread_sleep_context_init(&thread->context.sleep, duration);
  thread_process_event(thread, THREAD_EVENT_SLEEP);
}

void* thread_join(thread_t* thread)
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
      thread_wait_on_queue_context_init(&current->context.wait_on_queue, &thread->waiters, &thread->waiters_spinlock);
      spinlock_unlock(&thread->state_lock);
      thread_process_event(current, THREAD_EVENT_BLOCK);
    }
  }
  return thread->context.terminated.retval;;
}
