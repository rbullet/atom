#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define RP2040_SPINLOCK_COUNT                  32

typedef volatile uint32_t hardware_spinlock_t;

extern hardware_spinlock_t* const hardware_spinlock0;
extern hardware_spinlock_t* const hardware_spinlock1;
extern hardware_spinlock_t* const hardware_spinlock2;
extern hardware_spinlock_t* const hardware_spinlock3;
extern hardware_spinlock_t* const hardware_spinlock4;
extern hardware_spinlock_t* const hardware_spinlock5;
extern hardware_spinlock_t* const hardware_spinlock6;
extern hardware_spinlock_t* const hardware_spinlock7;
extern hardware_spinlock_t* const hardware_spinlock8;
extern hardware_spinlock_t* const hardware_spinlock9;
extern hardware_spinlock_t* const hardware_spinlock10;
extern hardware_spinlock_t* const hardware_spinlock11;
extern hardware_spinlock_t* const hardware_spinlock12;
extern hardware_spinlock_t* const hardware_spinlock13;
extern hardware_spinlock_t* const hardware_spinlock14;
extern hardware_spinlock_t* const hardware_spinlock15;
extern hardware_spinlock_t* const hardware_spinlock16;
extern hardware_spinlock_t* const hardware_spinlock17;
extern hardware_spinlock_t* const hardware_spinlock18;
extern hardware_spinlock_t* const hardware_spinlock19;
extern hardware_spinlock_t* const hardware_spinlock20;
extern hardware_spinlock_t* const hardware_spinlock21;
extern hardware_spinlock_t* const hardware_spinlock22;
extern hardware_spinlock_t* const hardware_spinlock23;
extern hardware_spinlock_t* const hardware_spinlock24;
extern hardware_spinlock_t* const hardware_spinlock25;
extern hardware_spinlock_t* const hardware_spinlock26;
extern hardware_spinlock_t* const hardware_spinlock27;
extern hardware_spinlock_t* const hardware_spinlock28;
extern hardware_spinlock_t* const hardware_spinlock29;
extern hardware_spinlock_t* const hardware_spinlock30;
extern hardware_spinlock_t* const hardware_spinlock31;

void hardware_spinlock_init(void);

void hardware_spinlock_lock(hardware_spinlock_t* hardware_spinlock);

void hardware_spinlock_unlock(hardware_spinlock_t* hardware_spinlock);

bool hardware_spinlock_try_lock(hardware_spinlock_t* hardware_spinlock);

/**
 * @cond INTERNAL
 */

static inline void hardware_spinlock_auto_unlock(hardware_spinlock_t** hardware_spinlock)
{
  hardware_spinlock_unlock(*hardware_spinlock);
}

/**
 * @endcond
 */

#define WITH_HARDWARE_SPINLOCK(spinlock)                                   \
    (void)({                                                               \
        hardware_spinlock_t* __hardware_spinlock__                         \
            __attribute__((cleanup(hardware_spinlock_auto_unlock)))        \
            = (hardware_spinlock_lock(spinlock), (spinlock));

#define WITH_HARDWARE_SPINLOCK_END (void)0;});

#ifdef __cplusplus
}
#endif
