#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/spinlock.h"
#include "concurrent/thread.h"
#include "rp2040/concurrent/deferred_task.h"
#include "util/time/timestamp.h"

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

void scheduler_thread_init(thread_t* thread, uint32_t* stack_base, size_t stack_size, thread_func_t start_routine, void* arg);

void scheduler_thread_start(thread_t* thread);

void scheduler_thread_resume(thread_t* thread);

void scheduler_thread_return_current_to_queue();

void scheduler_thread_yield_current();

void scheduler_thread_block_current();

void scheduler_thread_block_current_on(list_t* wait_queue, spinlock_t* guarded_by);

void* scheduler_thread_join(thread_t* thread);

void scheduler_thread_sleep_current(duration_t duration);

void scheduler_task_schedule(deferred_task_t* task);

void scheduler_task_cancel(deferred_task_t* deferred_task);

#ifdef __cplusplus
}
#endif
