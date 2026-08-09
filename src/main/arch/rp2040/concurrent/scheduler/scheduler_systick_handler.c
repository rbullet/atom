#include "internal.h"

volatile uint64_t sys_tick = 0;

static void scheduler_process_deferred_tasks(void)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&deferred_task_context.spinlock)
    {
      if (!sorted_list_is_empty(&deferred_task_context.tasks_queue))
      {
        deferred_task_t const* const deferred_task = CONTAINER_OF(deferred_task_context.tasks_queue.head, deferred_task_t, scheduler_node);
        if (scheduler_timestamp_is_expired(deferred_task->deadline))
        {
          event_flags_set(&deferred_task_context.event_flags, DEFERRED_TASK_EXPIRED);
        }
      }
    }
  }
}

static bool scheduler_should_yield(void)
{
  bool should_yield = false;
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&execution_context[CPUID].spinlock)
    {
      thread_t const* const current = execution_context[CPUID].current_thread;
      should_yield = current->state != THREAD_RUNNING || current == execution_context[CPUID].idle_thread || scheduler_timestamp_is_expired(current->deadline);
    }
  }
  return should_yield;
}

void scheduler_sys_tick_handler(void)
{
  if (CPU_IS_CORE_0)
  {
    sys_tick++;
    scheduler_process_deferred_tasks();
  }
  if (scheduler_should_yield())
  {
    scheduler_yield();
  }
}
