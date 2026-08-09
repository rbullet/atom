#pragma once
#ifdef __cplusplus
extern "C" {

#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "util/helpers.h"

// --- SIO peripheral registers (from RP2040 SVD) ---
#define SIO_BASE 0XD0000000
#define SIO_CPUID_OFFSET 0X0000
#define SIO_FIFO_ST_OFFSET 0X0050
#define SIO_FIFO_ST_VLD_MASK 0X1
#define SIO_FIFO_ST_VLD_OFFSET 0
#define SIO_FIFO_ST_RDY_MASK 0X2
#define SIO_FIFO_ST_RDY_OFFSET 1
#define SIO_FIFO_RD_OFFSET 0X0058
#define SIO_FIFO_WR_OFFSET 0X0054

#define SIO_FIFO_ST     ((volatile uint32_t* const)(SIO_BASE + SIO_FIFO_ST_OFFSET))
#define SIO_FIFO_RD     ((volatile uint32_t* const)(SIO_BASE + SIO_FIFO_RD_OFFSET))
#define SIO_FIFO_WR     ((volatile uint32_t* const)(SIO_BASE + SIO_FIFO_WR_OFFSET))

#define CORE_COUNT 2
#define CPUID           (*((volatile uint32_t*)(SIO_BASE + SIO_CPUID_OFFSET)))
#define CPU_IS_CORE_0   (CPUID == 0)   // Check if running on core 0
#define CPU_IS_CORE_1   (CPUID == 1)   // Check if running on core 1

// --- CONTROL register bit definitions ---
#define CPU_CONTROL_SPSEL_BIT 1

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
static __attribute__((always_inline)) inline uint32_t cpu_control(void)
{
  uint32_t value;
  __asm__ volatile("mrs %0, control" : "=r"(value));
  return value;
}

// --- Set stack pointer mode (MSP/PSP) ---
static __attribute__((always_inline)) inline void cpu_stack_set_mode(stack_mode_enum mode)
{
  uint32_t const old_value = cpu_control();
  uint32_t const new_value = (old_value & ~(1u << CPU_CONTROL_SPSEL_BIT)) | (mode << CPU_CONTROL_SPSEL_BIT);
  __asm__ volatile("msr control, %0" :: "r"(new_value) : "memory");
}

// --- CPU FIFO status helpers ---
static inline bool cpu_fifo_is_readable(void)
{
  return REG_GET_FIELD(SIO_FIFO_ST, SIO_FIFO_ST_VLD);
}

static inline bool cpu_fifo_is_writable(void)
{
  return REG_GET_FIELD(SIO_FIFO_ST, SIO_FIFO_ST_RDY);
}

// --- CPU FIFO blocking write ---
static inline size_t cpu_fifo_write(uint32_t const* buffer, size_t const len)
{
  for (size_t i = 0; i < len; i++)
  {
    while (!cpu_fifo_is_writable())
    {
      wfe();
    }

    *SIO_FIFO_WR = buffer[i];
    sev();
  }

  return len;
}

// --- CPU FIFO blocking read ---
static inline size_t cpu_fifo_read(uint32_t* buffer, size_t const len)
{
  for (size_t i = 0; i < len; i++)
  {
    while (!cpu_fifo_is_readable())
    {
      wfe();
    }
    buffer[i] = *SIO_FIFO_RD;
    sev();
  }
  return len;
}

// --- CPU FIFO non-blocking try read ---
static inline bool cpu_fifo_try_read(uint32_t* value)
{
  if (!cpu_fifo_is_readable())
  {
    return false;
  }

  *value = *SIO_FIFO_RD;
  return true;
}

// --- CPU FIFO non-blocking try write ---
static inline bool cpu_fifo_try_write(uint32_t const value)
{
  if (!cpu_fifo_is_writable())
  {
    return false;
  }

  *SIO_FIFO_WR = value;
  sev();
  return true;
}

// --- CPU FIFO flush (discard all pending reads) ---
static inline void cpu_fifo_flush(void)
{
  uint32_t dummy;
  while (cpu_fifo_try_read(&dummy))
  {
    __asm volatile("nop");
  }
  sev();
}

static inline bool cpu_fifo_write_echoed(uint32_t const* buffer, size_t len)
{
  uint32_t echo;

  cpu_fifo_flush();

  for (size_t i = 0; i < len; i++)
  {
    cpu_fifo_write(&buffer[i], 1);
    cpu_fifo_read(&echo, 1);

    if (echo != buffer[i])
    {
      return false;
    }
  }

  return true;
}

#ifdef __cplusplus
}
#endif
