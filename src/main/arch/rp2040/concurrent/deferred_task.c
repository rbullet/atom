#include <atom.h>

#include "rp2040/concurrent/deferred_task.h"
#include "rp2040/concurrent/scheduler.h"

static duration_t const one_shot = DURATION_INITIALIZER(0, MILLISECONDS);

void deferred_task_start_after(deferred_task_t* deferred_task, duration_t const initial_delay, deferred_task_callback_t const callback, void* arg)
{
  deferred_task_start_periodic(deferred_task, initial_delay, one_shot, callback, arg);
}

void deferred_task_start_periodic(deferred_task_t* deferred_task, duration_t const initial_delay, duration_t const period, deferred_task_callback_t const callback, void* arg)
{
  deferred_task->state = DEFERRED_TASK_IDLE;
  deferred_task->mutex = MUTEX_INITIALIZER;
  deferred_task->completion = CONDITION_VARIABLE_INITIALIZER;
  deferred_task->initial_delay = initial_delay;
  deferred_task->period = period;
  deferred_task->callback = callback;
  deferred_task->arg = arg;
  scheduler_deferred_task_start(deferred_task);
}

bool deferred_task_is_periodic(deferred_task_t const* deferred_task)
{
  return deferred_task->period.value != one_shot.value;
}

void deferred_task_cancel(deferred_task_t* deferred_task)
{
  scheduler_deferred_task_cancel(deferred_task);
}
