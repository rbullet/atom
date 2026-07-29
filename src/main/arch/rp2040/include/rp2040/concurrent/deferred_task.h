#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/deferred_task.h"

bool deferred_task_is_periodic(deferred_task_t const* deferred_task);

#ifdef __cplusplus
}
#endif
