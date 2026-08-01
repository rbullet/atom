#pragma once

/**
 * @file spinlock.h
 * @brief RP2040 spinlock allocator definitions.
 *
 * This private header exposes the RP2040 spinlock allocation policy used by
 * the ATOM port. It is intended exclusively for internal architecture use.
 *
 * Hardware spinlocks must be accessed through the spinlock pool allocator.
 * Direct use of hardware spinlock registers is not permitted outside of the
 * spinlock implementation.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/spinlock.h"

/**
 * @name RP2040 spinlock allocation layout
 *
 * The RP2040 provides 32 hardware spinlocks. ATOM manages these resources
 * through a central allocator.
 *
 * The allocation regions are:
 *
 * - Exclusive resources:
 *   Locks with stable ownership reserved for components requiring a dedicated
 *   spinlock. This includes ATOM internal users and user requested exclusive
 *   allocations.
 *
 * - Pooled resources:
 *   Locks allocated dynamically for synchronization objects created at runtime.
 *
 * One lock is reserved internally by the allocator to protect the spinlock
 * pool itself and is never returned by the allocator.
 *
 * @{
 */

#define RP2040_SPINLOCK_COUNT                  32

#define RP2040_SPINLOCK_POOL_LOCK_COUNT         1

#define RP2040_SPINLOCK_EXCLUSIVE_SYSTEM_COUNT  8

#define RP2040_SPINLOCK_EXCLUSIVE_USER_COUNT   10

#define RP2040_SPINLOCK_EXCLUSIVE_COUNT (RP2040_SPINLOCK_EXCLUSIVE_SYSTEM_COUNT + RP2040_SPINLOCK_EXCLUSIVE_USER_COUNT)

#define RP2040_SPINLOCK_POOLED_COUNT (RP2040_SPINLOCK_COUNT - RP2040_SPINLOCK_POOL_LOCK_COUNT - RP2040_SPINLOCK_EXCLUSIVE_COUNT)

#define RP2040_SPINLOCK_EXCLUSIVE_START 0

#define RP2040_SPINLOCK_POOLED_START (RP2040_SPINLOCK_EXCLUSIVE_START + RP2040_SPINLOCK_EXCLUSIVE_COUNT)

#define RP2040_SPINLOCK_POOL_END (RP2040_SPINLOCK_POOLED_START + RP2040_SPINLOCK_POOLED_COUNT)

/** @} */

void spinlock_hardware_unlock_all(void);

#ifdef __cplusplus
}
#endif