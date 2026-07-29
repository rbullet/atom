#pragma once

/**
 * @file spinlock.h
 * @brief RP2040 hardware spinlock definitions.
 *
 * This private header exposes the RP2040 hardware spinlock registers used by
 * the ATOM RP2040 port. These symbols are intended exclusively for internal
 * use by the RP2040 architecture implementation and are not part of the
 * public ATOM API.
 *
 * Hardware spinlocks may be reserved by ATOM subsystems to provide efficient
 * mutual exclusion between CPU cores. Reserved spinlocks must not be reused
 * outside of their designated subsystem.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/spinlock.h"

/** @name RP2040 hardware spinlocks
 *
 * Hardware spinlock registers provided by the RP2040 SIO peripheral.
 *
 * Spinlocks 0-24 are currently available for general internal use by the
 * RP2040 port. Spinlocks 25-31 are reserved by ATOM subsystems.
 *
 * @{
 */

/** @brief Hardware spinlock 0. */
extern spinlock_t* const spinlock0;

/** @brief Hardware spinlock 1. */
extern spinlock_t* const spinlock1;

/** @brief Hardware spinlock 2. */
extern spinlock_t* const spinlock2;

/** @brief Hardware spinlock 3. */
extern spinlock_t* const spinlock3;

/** @brief Hardware spinlock 4. */
extern spinlock_t* const spinlock4;

/** @brief Hardware spinlock 5. */
extern spinlock_t* const spinlock5;

/** @brief Hardware spinlock 6. */
extern spinlock_t* const spinlock6;

/** @brief Hardware spinlock 7. */
extern spinlock_t* const spinlock7;

/** @brief Hardware spinlock 8. */
extern spinlock_t* const spinlock8;

/** @brief Hardware spinlock 9. */
extern spinlock_t* const spinlock9;

/** @brief Hardware spinlock 10. */
extern spinlock_t* const spinlock10;

/** @brief Hardware spinlock 11. */
extern spinlock_t* const spinlock11;

/** @brief Hardware spinlock 12. */
extern spinlock_t* const spinlock12;

/** @brief Hardware spinlock 13. */
extern spinlock_t* const spinlock13;

/** @brief Hardware spinlock 14. */
extern spinlock_t* const spinlock14;

/** @brief Hardware spinlock 15. */
extern spinlock_t* const spinlock15;

/** @brief Hardware spinlock 16. */
extern spinlock_t* const spinlock16;

/** @brief Hardware spinlock 17. */
extern spinlock_t* const spinlock17;

/** @brief Hardware spinlock 18. */
extern spinlock_t* const spinlock18;

/** @brief Hardware spinlock 19. */
extern spinlock_t* const spinlock19;

/** @brief Hardware spinlock 20. */
extern spinlock_t* const spinlock20;

/** @brief Hardware spinlock 21. */
extern spinlock_t* const spinlock21;

/** @brief Hardware spinlock 22. */
extern spinlock_t* const spinlock22;

/** @brief Hardware spinlock 23. */
extern spinlock_t* const spinlock23;

/** @brief Hardware spinlock 24. */
extern spinlock_t* const spinlock24;

/** @brief Reserved for the condition variable subsystem. */
extern spinlock_t* const spinlock25;

/** @brief Reserved for the semaphore subsystem. */
extern spinlock_t* const spinlock26;

/** @brief Reserved for the mutex subsystem. */
extern spinlock_t* const spinlock27;

/** @brief Reserved for the deferred task subsystem. */
extern spinlock_t* const spinlock28;

/** @brief Reserved for the thread subsystem. */
extern spinlock_t* const spinlock29;

/** @brief Reserved for the scheduler subsystem. */
extern spinlock_t* const spinlock30;

/** @brief Reserved for the memory allocator. */
extern spinlock_t* const spinlock31;

/** @} */

#ifdef __cplusplus
}
#endif