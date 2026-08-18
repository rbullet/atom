#include <atom.h>
#include <string.h>

#include "rp2040/concurrent/scheduler.h"
#include "rp2040/concurrent/thread.h"

static bool message_queue_push_internal(message_queue_t* message_queue, void const* message, bool const blocking, duration_t const* const timeout)
{
  thread_t* const current_thread = thread_current();

  WITH_INTERRUPTS_DISABLED
    {
      bool inserted = false;
      spinlock_lock(&message_queue->spinlock);

      while (!list_is_empty(&message_queue->receivers))
      {
        thread_t* const receiver = CONTAINER_OF(message_queue->receivers.head, thread_t, scheduler_node);

        spinlock_unlock(&message_queue->spinlock);

        bool awoken = false;

        WITH_SPINLOCK(&receiver->state_lock)
          {
            if (receiver->context.timeout.wakeup_state != THREAD_WAKEUP_TIMED_OUT && receiver->context.timeout.wakeup_state != THREAD_WAKEUP_AWOKEN)
            {
              awoken = true;
              receiver->context.timeout.wakeup_state = THREAD_WAKEUP_AWOKEN;
            }
          }
        WITH_SPINLOCK_END

        if (!awoken)
        {
          spinlock_lock(&message_queue->spinlock);

          continue;
        }

        void* const receiver_message_buffer = receiver->context.wait.custom_param;

        memcpy(receiver_message_buffer, message, message_queue->buffer.message_size);

        scheduler_state_machine_process_event(receiver, THREAD_EVENT_WAKEUP);

        return true;
      }

      inserted = ring_buffer_push(&message_queue->buffer, message);

      if (inserted)
      {
        spinlock_unlock(&message_queue->spinlock);
        return true;
      }

      if (!blocking)
      {
        spinlock_unlock(&message_queue->spinlock);
        return false;
      }

      if (timeout == NULL)
      {
        thread_context_wait_on_queue_with_custom_param_init(&current_thread->context, &message_queue->senders, &message_queue->spinlock, (void*)message);
      }
      else
      {
        thread_context_wait_on_queue_with_custom_param_and_timeout_init(&current_thread->context, &message_queue->senders, &message_queue->spinlock, (void*)message, *timeout);
      }

      scheduler_state_machine_process_event(current_thread, THREAD_EVENT_BLOCK);
    }
  WITH_INTERRUPTS_DISABLED_END

  return timeout == NULL || current_thread->context.timeout.wakeup_state == THREAD_WAKEUP_AWOKEN;
}

bool message_queue_try_push(message_queue_t* message_queue, void const* message)
{
  return message_queue_push_internal(message_queue, message, false, NULL);
}

void message_queue_push(message_queue_t* message_queue, void const* message)
{
  (void)message_queue_push_internal(message_queue, message, true, NULL);
}

bool message_queue_push_with_timeout(message_queue_t* message_queue, void const* message, duration_t const timeout)
{
  return message_queue_push_internal(message_queue, message,true, &timeout);
}

static bool message_queue_pop_internal(message_queue_t* message_queue, void* message, bool const blocking, duration_t const* const timeout)
{
  thread_t* const current_thread = thread_current();

  WITH_INTERRUPTS_DISABLED
    {
      bool retrieved = false;
      spinlock_lock(&message_queue->spinlock);

      retrieved = ring_buffer_pop(&message_queue->buffer, message);

      if (retrieved)
      {
        while (!list_is_empty(&message_queue->senders))
        {
          thread_t* const sender = CONTAINER_OF(message_queue->senders.head, thread_t, scheduler_node);

          spinlock_unlock(&message_queue->spinlock);

          bool awoken = false;

          WITH_SPINLOCK(&sender->state_lock)
            {
              if (sender->context.timeout.wakeup_state != THREAD_WAKEUP_TIMED_OUT && sender->context.timeout.wakeup_state != THREAD_WAKEUP_AWOKEN)
              {
                awoken = true;
                sender->context.timeout.wakeup_state = THREAD_WAKEUP_AWOKEN;
              }
            }
          WITH_SPINLOCK_END

          if (!awoken)
          {
            spinlock_lock(&message_queue->spinlock);
            continue;
          }

          void const* const sender_message = sender->context.wait.custom_param;

          spinlock_lock(&message_queue->spinlock);
          ring_buffer_push(&message_queue->buffer, sender_message);
          spinlock_unlock(&message_queue->spinlock);

          scheduler_state_machine_process_event(sender, THREAD_EVENT_WAKEUP);

          return true;
        }

        spinlock_unlock(&message_queue->spinlock);
        return true;
      }

      if (!blocking)
      {
        spinlock_unlock(&message_queue->spinlock);
        return false;
      }

      if (timeout == NULL)
      {
        thread_context_wait_on_queue_with_custom_param_init(&current_thread->context, &message_queue->receivers, &message_queue->spinlock, message);
      }
      else
      {
        thread_context_wait_on_queue_with_custom_param_and_timeout_init(&current_thread->context, &message_queue->receivers, &message_queue->spinlock, message, *timeout);
      }

      scheduler_state_machine_process_event(current_thread, THREAD_EVENT_BLOCK);
    }
  WITH_INTERRUPTS_DISABLED_END

  return timeout == NULL || current_thread->context.timeout.wakeup_state == THREAD_WAKEUP_AWOKEN;
}

bool message_queue_try_pop(message_queue_t* message_queue, void* message)
{
  return message_queue_pop_internal(message_queue, message, false, NULL);
}

void message_queue_pop(message_queue_t* message_queue, void* message)
{
  (void)message_queue_pop_internal(message_queue, message, true, NULL);
}

bool message_queue_pop_with_timeout(message_queue_t* message_queue, void* message, duration_t const timeout)
{
  return message_queue_pop_internal(message_queue, message, true, &timeout);
}

bool message_queue_is_empty(message_queue_t const* message_queue)
{
  return ring_buffer_is_empty(&message_queue->buffer);
}

bool message_queue_is_full(message_queue_t const* message_queue)
{
  return ring_buffer_is_full(&message_queue->buffer);
}

size_t message_queue_count(message_queue_t const* message_queue)
{
  return ring_buffer_count(&message_queue->buffer);
}

size_t message_queue_capacity(message_queue_t const* message_queue)
{
  return message_queue->buffer.capacity;
}
