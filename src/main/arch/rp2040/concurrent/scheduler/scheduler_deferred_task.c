#include "internal.h"

static bool deferred_task_expiration_comparator(list_node_t const* left, list_node_t const* right);

deferred_task_context_t deferred_task_context = {
  .spinlock = SPINLOCK_INITIALIZER,
  .tasks_queue = SORTED_LIST_INITIALIZER(deferred_task_expiration_comparator),
  .event_flags = EVENT_FLAGS_INITIALIZER,
  .thread = NULL
};

void* __attribute__((noreturn)) scheduler_deferred_task_worker(void* const arg)
{
  (void)arg;
  for (;;)
  {
    event_flags_wait(&deferred_task_context.event_flags, DEFERRED_TASK_EXPIRED, EVENT_FLAGS_ALL_SET);

    list_t expired_tasks = LIST_INITIALIZER;

    WITH_INTERRUPTS_DISABLED
      {
        WITH_SPINLOCK(&deferred_task_context.spinlock)
          {
            while (!sorted_list_is_empty(&deferred_task_context.tasks_queue))
            {
              deferred_task_t* deferred_task = CONTAINER_OF(deferred_task_context.tasks_queue.head, deferred_task_t, scheduler_node);
              if (!scheduler_timestamp_is_expired(deferred_task->deadline))
              {
                break;
              }
              sorted_list_remove(&deferred_task_context.tasks_queue, &deferred_task->scheduler_node);
              list_push(&expired_tasks, &deferred_task->scheduler_node);
            }
            event_flags_clear(&deferred_task_context.event_flags, DEFERRED_TASK_EXPIRED);
          }
        WITH_SPINLOCK_END
      }
    WITH_INTERRUPTS_DISABLED_END

    while (!list_is_empty(&expired_tasks))
    {
      deferred_task_t* const deferred_task = CONTAINER_OF(list_pop(&expired_tasks), deferred_task_t, scheduler_node);

      bool should_run = false;
      WITH_MUTEX(&deferred_task->mutex)
        {
          if (deferred_task->state == DEFERRED_TASK_SCHEDULED)
          {
            deferred_task->state = DEFERRED_TASK_RUNNING;
            should_run = true;
          }
        }
      WITH_MUTEX_END

      if (!should_run)
      {
        continue;
      }

      deferred_task->callback(deferred_task->arg);
      WITH_MUTEX(&deferred_task->mutex)
        {
          if (deferred_task->state == DEFERRED_TASK_CANCELLED)
          {
            condition_variable_broadcast(&deferred_task->completion);
          }
          else if (deferred_task_is_periodic(deferred_task))
          {
            WITH_INTERRUPTS_DISABLED
              {
                WITH_SPINLOCK(&deferred_task_context.spinlock)
                  {
                    deferred_task->state = DEFERRED_TASK_SCHEDULED;
                    deferred_task->deadline = scheduler_timestamp_add(deferred_task->deadline, deferred_task->period);
                    sorted_list_add(&deferred_task_context.tasks_queue, &deferred_task->scheduler_node);
                  }
                WITH_SPINLOCK_END
              }
            WITH_INTERRUPTS_DISABLED_END
          }
          else
          {
            deferred_task->state = DEFERRED_TASK_COMPLETED;
            condition_variable_broadcast(&deferred_task->completion);
          }
        }
      WITH_MUTEX_END
    }
  }
}

void scheduler_deferred_task_start(deferred_task_t* const deferred_task)
{
  WITH_MUTEX(&deferred_task->mutex)
    {
      if (deferred_task->state != DEFERRED_TASK_IDLE)
      {
        return;
      }
      deferred_task->scheduler_node = LIST_NODE_INITIALIZER;
      deferred_task->state = DEFERRED_TASK_SCHEDULED;
      deferred_task->deadline = scheduler_timestamp_add(scheduler_timestamp_now(), deferred_task->initial_delay);
      WITH_INTERRUPTS_DISABLED
        {
          WITH_SPINLOCK(&deferred_task_context.spinlock)
            {
#ifdef DEBUG
              ATOM_ASSERT(deferred_task->scheduler_node.next == NULL && deferred_task->scheduler_node.previous == NULL, "Deferred task already linked");
#endif
              sorted_list_add(&deferred_task_context.tasks_queue, &deferred_task->scheduler_node);
            }
          WITH_SPINLOCK_END
        }
      WITH_INTERRUPTS_DISABLED_END
    }
  WITH_MUTEX_END
}

void scheduler_deferred_task_cancel(deferred_task_t* deferred_task)
{
  WITH_MUTEX(&deferred_task->mutex)
    {
      switch (deferred_task->state)
      {
      case DEFERRED_TASK_SCHEDULED:
        deferred_task->state = DEFERRED_TASK_CANCELLED;
        WITH_INTERRUPTS_DISABLED
          {
            WITH_SPINLOCK(&deferred_task_context.spinlock)
              {
                if (sorted_list_contains(&deferred_task_context.tasks_queue, &deferred_task->scheduler_node))
                {
                  sorted_list_remove(&deferred_task_context.tasks_queue, &deferred_task->scheduler_node);
                }
              }
            WITH_SPINLOCK_END
          }
        WITH_INTERRUPTS_DISABLED_END
        break;

      case DEFERRED_TASK_RUNNING:
        deferred_task->state = DEFERRED_TASK_CANCELLED;
        if (execution_context[CPUID].current_thread != deferred_task_context.thread)
        {
          condition_variable_wait(&deferred_task->completion, &deferred_task->mutex);
        }
        break;

      default:
        break;
      }
    }
  WITH_MUTEX_END
}

static bool deferred_task_expiration_comparator(list_node_t const* left, list_node_t const* right)
{
  deferred_task_t const* first = CONTAINER_OF(left, deferred_task_t, scheduler_node);
  deferred_task_t const* second = CONTAINER_OF(right, deferred_task_t, scheduler_node);
  return scheduler_timestamp_is_before(first->deadline, second->deadline);
}
