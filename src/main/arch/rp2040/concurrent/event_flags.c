#include "rp2040/atom.h"

typedef struct
{
  event_flags_mask_t mask;
  event_flags_mode_t mode;
} event_flags_wait_param_t;

#define EVENT_FLAGS_WAIT_PARAM_INITIALIZER(msk, m) ((event_flags_wait_param_t){ .mask = (msk), .mode = (m) })

static inline bool is_condition_met(event_flags_t const* event, event_flags_mask_t const mask, event_flags_mode_t const mode)
{
  switch (mode)
  {
  case EVENT_FLAGS_ALL_SET:
    return (event->flags & mask) == mask;

  case EVENT_FLAGS_ANY_SET:
    return (event->flags & mask) != 0;

  default:
    ATOM_ASSERT(false, "Unsupported event flags mode");
  }

  return false;
}

void event_flags_set(event_flags_t* event, event_flags_mask_t const flags)
{
  list_t resume_list = LIST_INITIALIZER;
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&event->spinlock)
    {
      BITS_SET(event->flags, flags);
      list_t waiter_list = LIST_INITIALIZER;
      while (!list_is_empty(&event->waiters))
      {
        thread_t* const thread = CONTAINER_OF(list_pop(&event->waiters), thread_t, scheduler_node);

        event_flags_wait_param_t const* const waiter = thread->context.wait_on_queue_with_custom_param.custom_param;

        if (is_condition_met(event, waiter->mask, waiter->mode))
        {
          list_push(&resume_list, &thread->scheduler_node);
        }
        else
        {
          list_push(&waiter_list, &thread->scheduler_node);
        }
      }
      event->waiters = waiter_list;
    }

    while (!list_is_empty(&resume_list))
    {
      thread_t* const thread = CONTAINER_OF(list_pop(&resume_list), thread_t, scheduler_node);
      scheduler_thread_process_event(thread, THREAD_EVENT_WAKEUP);
    }
  }
}

void event_flags_clear(event_flags_t* event, event_flags_mask_t const flags)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&event->spinlock)
    {
      BITS_CLEAR(event->flags, flags);
    }
  }
}

void event_flags_wait(event_flags_t* event, event_flags_mask_t const mask, event_flags_mode_t const mode)
{
  thread_t* const thread = thread_current();

  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(&event->spinlock);
    if (is_condition_met(event, mask, mode))
    {
      spinlock_unlock(&event->spinlock);
      return;
    }

    event_flags_wait_param_t waiter = EVENT_FLAGS_WAIT_PARAM_INITIALIZER(mask, mode);
    thread_wait_on_queue_with_custom_param_init(&thread->context.wait_on_queue_with_custom_param, &event->waiters, &event->spinlock, &waiter);
    scheduler_thread_process_event(thread, THREAD_EVENT_BLOCK);
  }
}

bool event_flags_try_wait(event_flags_t* event, event_flags_mask_t const mask, event_flags_mode_t const mode)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&event->spinlock)
    {
      if (is_condition_met(event, mask, mode))
      {
        return true;
      }
    }
  }
  return false;
}
