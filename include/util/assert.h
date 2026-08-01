#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
#define ATOM_ASSERT(condition, fmt, ...)                                    \
do                                                                          \
{                                                                           \
    if (!(condition))                                                       \
    {                                                                       \
        log_fatal("Assertion '%s' failed: " fmt,                            \
                  #condition,                                               \
                  ##__VA_ARGS__);                                           \
        cpu_breakpoint();                                                   \
        for (;;);                                                           \
    }                                                                       \
} while (0)
#else
#define ATOM_ASSERT(condition, fmt, ...) ((void)0)
#endif

/**
 * @brief Trigger a processor breakpoint.
 *
 * Causes execution to halt when a debugger is attached.
 *
 * The implementation is architecture-specific and is used internally by
 * ATOM assertion handling.
 *
 * @note This function may have no effect when no debugger is attached,
 * depending on the target architecture.
 */
void cpu_breakpoint(void);

#ifdef __cplusplus
}
#endif