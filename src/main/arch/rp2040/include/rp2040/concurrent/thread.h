#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/thread.h"

/**
 * @cond INTERNAL
 */

static inline void wait_param_auto_restore(void** param)
{
  thread_current()->wait_param = *param;
}

#define _THREAD_WITH_WAIT_PARAM_BLOCK_WITH_ID(param, ID)                         \
for (bool _CAT(_wait_param_once_, ID) = true;                                    \
     _CAT(_wait_param_once_, ID);                                                \
     _CAT(_wait_param_once_, ID) = false)                                        \
    for (void* _CAT(_wait_param_value_, ID)                                      \
             __attribute__((cleanup(wait_param_auto_restore))) =                 \
             (thread_current()->wait_param);                                     \
         _CAT(_wait_param_once_, ID);                                            \
         _CAT(_wait_param_once_, ID) = false)                                    \
        for (thread_current()->wait_param = (void*)(param);                      \
             _CAT(_wait_param_once_, ID);                                        \
             _CAT(_wait_param_once_, ID) = false)

/**
 * @endcond
 */

#define THREAD_WITH_WAIT_PARAM(param) _THREAD_WITH_WAIT_PARAM_BLOCK_WITH_ID(param, __COUNTER__)

#ifdef __cplusplus
}
#endif
