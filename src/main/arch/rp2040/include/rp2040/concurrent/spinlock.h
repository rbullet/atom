#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "rp2040/rp2040.h"
#include "concurrent/spinlock.h"

// --- RP2040 hardware spinlocks ---
#define spinlock0    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK0_OFFSET))
#define spinlock1    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK1_OFFSET))
#define spinlock2    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK2_OFFSET))
#define spinlock3    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK3_OFFSET))
#define spinlock4    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK4_OFFSET))
#define spinlock5    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK5_OFFSET))
#define spinlock6    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK6_OFFSET))
#define spinlock7    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK7_OFFSET))
#define spinlock8    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK8_OFFSET))
#define spinlock9    ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK9_OFFSET))
#define spinlock10   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK10_OFFSET))
#define spinlock11   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK11_OFFSET))
#define spinlock12   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK12_OFFSET))
#define spinlock13   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK13_OFFSET))
#define spinlock14   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK14_OFFSET))
#define spinlock15   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK15_OFFSET))
#define spinlock16   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK16_OFFSET))
#define spinlock17   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK17_OFFSET))
#define spinlock18   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK18_OFFSET))
#define spinlock19   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK19_OFFSET))
#define spinlock20   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK20_OFFSET))
#define spinlock21   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK21_OFFSET))
#define spinlock22   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK22_OFFSET))
#define spinlock23   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK23_OFFSET))
#define spinlock24   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK24_OFFSET))
#define spinlock25   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK25_OFFSET))
#define spinlock26   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK26_OFFSET))
#define spinlock27   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK27_OFFSET))
#define spinlock28   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK28_OFFSET))
#define spinlock29   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK29_OFFSET))
#define spinlock30   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK30_OFFSET))
#define spinlock31   ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK31_OFFSET))

/** @endcond */

#ifdef __cplusplus
}
#endif
