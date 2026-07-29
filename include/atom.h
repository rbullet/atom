#pragma once

/**
 * @file atom.h
 * @brief Main public header for the ATOM framework.
 *
 * This header provides the complete public API of the ATOM framework.
 * Including this file makes all public modules available without requiring
 * individual header includes.
 *
 * ATOM is a lightweight embedded framework providing:
 * - Cooperative and preemptive thread scheduling
 * - Synchronization primitives
 * - Deferred task execution
 * - Portable hardware abstraction
 * - Time utilities
 * - Collection containers
 * - Logging and assertion facilities
 *
 * Applications may simply include this header:
 *
 * @code{.c}
 * #include <atom.h>
 * @endcode
 *
 * Advanced users may instead include only the specific module headers they
 * require to reduce compilation dependencies.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup atom ATOM
 * @brief Lightweight embedded framework.
 *
 * ATOM provides a portable API for embedded systems including concurrency,
 * hardware abstraction, utility containers and time management.
 *
 * The framework is organized into several modules:
 *
 * - @ref concurrent "Concurrency primitives"
 * - @ref io "Hardware I/O"
 * - @ref util "Utility library"
 *
 * This header includes every public API exposed by the framework.
 *
 * @{
 */

/* -------------------------------------------------------------------------- */
/* Configuration                                                               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Framework configuration.
 *
 * Compile-time configuration options for the ATOM framework.
 */
#include "atom_config.h"

/* -------------------------------------------------------------------------- */
/* Concurrency                                                                 */
/* -------------------------------------------------------------------------- */

/**
 * @name Concurrency
 * Thread scheduling and synchronization primitives.
 * @{
 */

#include "concurrent/condition_variable.h"
#include "concurrent/deferred_task.h"
#include "concurrent/interrupts.h"
#include "concurrent/mutex.h"
#include "concurrent/semaphore.h"
#include "concurrent/spinlock.h"
#include "concurrent/thread.h"

/** @} */

/* -------------------------------------------------------------------------- */
/* Input / Output                                                              */
/* -------------------------------------------------------------------------- */

/**
 * @name Hardware I/O
 * Portable hardware abstraction interfaces.
 * @{
 */

#include "io/gpio.h"
#include "io/uart.h"

/** @} */

/* -------------------------------------------------------------------------- */
/* Collections                                                                 */
/* -------------------------------------------------------------------------- */

/**
 * @name Collections
 * Generic container data structures.
 * @{
 */

#include "util/collection/list.h"
#include "util/collection/sorted_list.h"

/** @} */

/* -------------------------------------------------------------------------- */
/* Time                                                                        */
/* -------------------------------------------------------------------------- */

/**
 * @name Time
 * Time representations and helper utilities.
 * @{
 */

#include "util/time/duration.h"
#include "util/time/time_unit.h"
#include "util/time/timestamp.h"

/** @} */

/* -------------------------------------------------------------------------- */
/* Utilities                                                                   */
/* -------------------------------------------------------------------------- */

/**
 * @name Utilities
 * Assertions, helper macros and logging.
 * @{
 */

#include "util/assert.h"
#include "util/helpers.h"
#include "util/log.h"

/** @} */

/** @} */ /* end of atom */

#ifdef __cplusplus
}
#endif