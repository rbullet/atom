#pragma once

/**
 * @file atom.h
 * @brief Internal RP2040 ATOM API aggregation header.
 *
 * This header provides access to the internal ATOM APIs required by the
 * RP2040 architecture implementation.
 *
 * It includes the generic ATOM public API as well as RP2040-specific
 * subsystems such as clock configuration, CPU management, interrupt handling,
 * scheduler services, deferred tasks, and hardware synchronization primitives.
 *
 * This header is intended for ATOM architecture implementation files only.
 * Application code should include the public "atom.h" header instead.
 *
 * Each supported architecture provides its own internal aggregation header
 * following the same pattern:
 *
 * @code
 * #include "<architecture>/atom.h"
 * @endcode
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "atom.h"


#include "rp2040/system/clocks.h"
#include "rp2040/system/cpu.h"
#include "rp2040/system/pll.h"
#include "rp2040/system/resets.h"
#include "rp2040/system/xosc.h"

#include "rp2040/concurrent/deferred_task.h"
#include "rp2040/concurrent/interrupts.h"
#include "rp2040/concurrent/scheduler.h"
#include "rp2040/concurrent/spinlock.h"

#ifdef __cplusplus
}
#endif

/** @} */