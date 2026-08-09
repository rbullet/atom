#pragma once
#ifdef __cplusplus
extern "C" {
#endif


#include "util/time/timestamp.h"
#include "concurrent/scheduler.h"
#include "concurrent/thread.h"

typedef enum
{
  THREAD_EVENT_NONE,
  THREAD_EVENT_START,
  THREAD_EVENT_RUN,
  THREAD_EVENT_YIELD,
  THREAD_EVENT_BLOCK,
  THREAD_EVENT_SLEEP,
  THREAD_EVENT_WAKEUP,
  THREAD_EVENT_TERMINATE,

  THREAD_EVENT_COUNT
} thread_event_t;

void scheduler_init(void);

timestamp_t scheduler_timestamp_now(void);

timestamp_t scheduler_timestamp_add(timestamp_t timestamp, duration_t duration);

bool scheduler_timestamp_is_before(timestamp_t first, timestamp_t second);

bool scheduler_timestamp_is_before_or_equal(timestamp_t first, timestamp_t other);

bool scheduler_timestamp_is_after(timestamp_t first, timestamp_t other);

bool scheduler_timestamp_is_after_or_equal(timestamp_t first, timestamp_t other);

bool scheduler_timestamp_is_expired(timestamp_t deadline);

duration_t scheduler_timestamp_duration_since(timestamp_t timestamp);

thread_t* scheduler_thread_current(void);

bool scheduler_state_machine_process_event(thread_t *thread, thread_event_t event);

void scheduler_thread_init(thread_t* thread, uint32_t* stack_base, size_t stack_size, thread_func_t start_routine, void* arg);

void scheduler_deferred_task_start(deferred_task_t* deferred_task);

void scheduler_deferred_task_cancel(deferred_task_t* deferred_task);

#ifdef __cplusplus
}
#endif
