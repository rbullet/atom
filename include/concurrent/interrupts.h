#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "util/helpers.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup interrupts Interrupts
 * @brief CPU interrupt masking and scoped interrupt control.
 *
 * Provides low-level primitives to temporarily enable or disable maskable
 * CPU interrupts and restore the previous interrupt state.
 *
 * These functions operate on the current CPU core only. On multi-core systems,
 * disabling interrupts does not prevent execution on other cores.
 *
 * For protecting data shared between cores, use spinlocks or higher-level
 * synchronization primitives.
 *
 * @ingroup concurrent
 * @{
 */

bool in_interrupt(void);

/**
 * @brief Check whether maskable interrupts are enabled.
 *
 * Returns the current interrupt mask state of the executing CPU core.
 *
 * @retval true  Maskable interrupts are enabled.
 * @retval false Maskable interrupts are disabled.
 *
 * @note Safe from thread and interrupt context.
 */
bool interrupts_are_enabled(void);


/**
 * @brief Enable maskable interrupts.
 *
 * Enables CPU maskable interrupts and returns the previous interrupt state.
 *
 * The returned state must be restored using interrupts_restore() when the
 * caller needs to return to the previous execution state.
 *
 * @return Previous interrupt state.
 *
 * @see interrupts_restore()
 * @see WITH_INTERRUPTS_ENABLED
 *
 * @note Safe from thread and interrupt context.
 */
uint32_t interrupts_enable(void);


/**
 * @brief Disable maskable interrupts.
 *
 * Disables CPU maskable interrupts and returns the previous interrupt state.
 *
 * The returned state must be restored using interrupts_restore() when the
 * critical section is complete.
 *
 * @return Previous interrupt state.
 *
 * @see interrupts_restore()
 * @see WITH_INTERRUPTS_DISABLED
 *
 * @note Safe from thread and interrupt context.
 *
 * @warning Disabling interrupts only affects the current CPU core.
 */
uint32_t interrupts_disable(void);


/**
 * @brief Restore a previously saved interrupt state.
 *
 * Restores an interrupt state previously returned by
 * interrupts_enable() or interrupts_disable().
 *
 * @param state Previously saved interrupt state.
 *
 * @pre state must originate from interrupts_enable() or
 *      interrupts_disable().
 *
 * @note Safe from thread and interrupt context.
 */
void interrupts_restore(uint32_t state);

/**
 * @cond INTERNAL
 */

static inline void interrupt_auto_restore_state(uint32_t const* state)
{
  interrupts_restore(*state);
}

/**
 * @endcond
 */


/**
 * @brief Execute a scope with interrupts enabled.
 *
 * Enables maskable interrupts on entry and automatically restores the previous
 * interrupt state when leaving the scope.
 *
 * Nested usage is supported because the previous state is saved and restored.
 *
 * Example:
 *
 * @code
 * WITH_INTERRUPTS_ENABLED
 * {
 *     process_pending_events();
 * }
 * WITH_INTERRUPTS_ENABLED_END
 * @endcode
 *
 * @warning This does not synchronize with the other CPU core.
 */
#define WITH_INTERRUPTS_ENABLED                                            \
    (void)({                                                               \
    uint32_t __irq_state__                                                 \
        __attribute__((cleanup(interrupt_auto_restore_state)))             \
        = interrupts_enable();

#define WITH_INTERRUPTS_ENABLED_END (void)0;});

/**
 * @brief Execute a scope with interrupts disabled.
 *
 * Disables maskable interrupts on entry and automatically restores the previous
 * interrupt state when leaving the scope.
 *
 * Example:
 *
 * @code
 * WITH_INTERRUPTS_DISABLED
 * {
 *     update_scheduler_state();
 * }
 * WITH_INTERRUPTS_DISABLED_END
 * @endcode
 *
 * Useful for protecting short sections of code that must not be interrupted
 * on the current CPU core.
 *
 * @warning Keep disabled sections very short to minimize interrupt latency.
 *
 * @warning This does not protect against concurrent access from another CPU
 *          core. Use spinlocks for SMP synchronization.
 */
#define WITH_INTERRUPTS_DISABLED                                       \
    (void)({                                                           \
        uint32_t __irq_state__                                         \
            __attribute__((cleanup(interrupt_auto_restore_state)))     \
            = interrupts_disable();

#define WITH_INTERRUPTS_DISABLED_END (void)0;});


/** @} */ /* end of interrupts */
/** @} */ /* end of concurrent */


#ifdef __cplusplus
}
#endif
