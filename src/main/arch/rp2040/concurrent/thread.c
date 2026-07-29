#include "rp2040/concurrent/scheduler.h"

void thread_init(thread_t* thread, uint32_t* stack_base, size_t const stack_size, thread_func_t const start_routine, void* arg)
{
  scheduler_thread_init(thread, stack_base, stack_size, start_routine, arg);
}

void thread_start(thread_t* thread)
{
  scheduler_thread_start(thread);
}

thread_t* thread_current(void)
{
  return scheduler_thread_current();
}

void thread_yield(void)
{
  scheduler_thread_yield_current();
}

void thread_wait(void)
{
  scheduler_thread_block_current();
}

void thread_notify(thread_t* thread)
{
  scheduler_thread_resume(thread);
}

void thread_sleep(duration_t const duration)
{
  scheduler_thread_sleep_current(duration);
}

void* thread_join(thread_t* thread)
{
  return scheduler_thread_join(thread);
}
