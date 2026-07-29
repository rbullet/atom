#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// --- CONTROL register bit definitions ---
#define CONTROL_SPSEL_BIT 1

// --- Stack mode selection ---
typedef enum
{
  STACK_MODE_MSP = 0U, ///< Main Stack Pointer
  STACK_MODE_PSP = 1U ///< Process Stack Pointer
} stack_mode_enum;

// --- Send Event (wake other cores) ---
__attribute__((always_inline)) static inline void sev(void)
{
  __asm__ volatile("sev" ::: "memory");
}

// --- Wait For Event (low-power sleep until event) ---
__attribute__((always_inline)) static inline void wfe(void)
{
  __asm__ volatile("wfe" ::: "memory");
}

// --- Wait For Interrupt (low-power sleep until interrupt) ---
__attribute__((always_inline)) static inline void wfi(void)
{
  __asm__ volatile("wfi" ::: "memory");
}

// --- Read CONTROL register ---
static __attribute__((always_inline)) inline uint32_t control(void)
{
  uint32_t value;
  __asm__ volatile("mrs %0, control" : "=r"(value));
  return value;
}

// --- Set stack pointer mode (MSP/PSP) ---
static __attribute__((always_inline)) inline void stack_set_mode(stack_mode_enum mode)
{
  uint32_t const old_value = control();
  uint32_t const new_value = (old_value & ~(1u << CONTROL_SPSEL_BIT)) | (mode << CONTROL_SPSEL_BIT);
  __asm__ volatile("msr control, %0" :: "r"(new_value) : "memory");
}

#ifdef __cplusplus
}
#endif
