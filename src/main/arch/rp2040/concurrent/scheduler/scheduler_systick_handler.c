#include "internal.h"

volatile uint64_t sys_tick = 0;

void scheduler_sys_tick_handler(void)
{
  if (CPU_IS_CORE_1)
  {
    __asm__ __volatile__("nop");
  }
  WITH_INTERRUPTS_DISABLED
  {
    if (CPU_IS_CORE_0)
    {
      sys_tick++;
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
    WITH_SPINLOCK(&execution_context[CPUID].spinlock)
    {
      if (
        execution_context[CPUID].current_thread->state != THREAD_RUNNING
        || execution_context[CPUID].current_thread == execution_context[CPUID].idle_thread
        || scheduler_timestamp_is_expired(execution_context[CPUID].current_thread->deadline)
      )
      {
        scheduler_request_context_switch();
      }
    }
  }
}
