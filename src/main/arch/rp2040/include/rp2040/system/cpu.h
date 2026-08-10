#pragma once
#ifdef __cplusplus
extern "C" {

#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "util/helpers.h"

// --- SIO peripheral registers (from RP2040 SVD) ---

#ifndef SIO_BASE
#define SIO_BASE                    0xD0000000U
#endif

#define SIO_CPUID_OFFSET            0x0000U

#define SIO_FIFO_ST_OFFSET          0x0050U
#define SIO_FIFO_WR_OFFSET          0x0054U
#define SIO_FIFO_RD_OFFSET          0x0058U

#define SIO_FIFO_ST_VLD_MASK        0x00000001U
#define SIO_FIFO_ST_VLD_OFFSET      0U
#define SIO_FIFO_ST_RDY_MASK        0x00000002U
#define SIO_FIFO_ST_RDY_OFFSET      1U

#define SIO_FIFO_ST                 REG(SIO_BASE, SIO_FIFO_ST_OFFSET)
#define SIO_FIFO_WR                 REG(SIO_BASE, SIO_FIFO_WR_OFFSET)
#define SIO_FIFO_RD                 REG(SIO_BASE, SIO_FIFO_RD_OFFSET)

// --- CPU identification ---

#define CORE_COUNT                  2U
#define CPUID                       REG_READ(REG(SIO_BASE, SIO_CPUID_OFFSET))
#define CPU_IS_CORE_0               (CPUID == 0U)
#define CPU_IS_CORE_1               (CPUID == 1U)

// --- CONTROL register bit definitions ---

#define CPU_CONTROL_SPSEL_BIT 1

// --- Stack mode selection ---

typedef enum
{
  STACK_MODE_MSP = 0U, ///< Main Stack Pointer
  STACK_MODE_PSP = 1U ///< Process Stack Pointer
} stack_mode_t;

// --- Event and wait instructions ---

__attribute__((always_inline)) static inline void sev(void)
{
  __asm volatile("sev" ::: "memory");
}

__attribute__((always_inline)) static inline void wfe(void)
{
  __asm volatile("wfe" ::: "memory");
}

__attribute__((always_inline)) static inline void wfi(void)
{
  __asm volatile("wfi" ::: "memory");
}

// --- CPU control register ---

static __attribute__((always_inline)) inline uint32_t cpu_control(void)
{
  uint32_t value;
  __asm volatile("mrs %0, control" : "=r"(value));
  return value;
}

// --- Stack pointer selection ---

static __attribute__((always_inline)) inline void cpu_stack_set_mode(stack_mode_t mode)
{
  uint32_t const old_value = cpu_control();
  uint32_t const new_value = (old_value & ~(1u << CPU_CONTROL_SPSEL_BIT)) | (mode << CPU_CONTROL_SPSEL_BIT);
  __asm volatile("msr control, %0" :: "r"(new_value) : "memory");
}

// --- Inter-core FIFO status ---

static inline bool cpu_fifo_is_readable(void)
{
  return REG_GET_FIELD(SIO_FIFO_ST, SIO_FIFO_ST_VLD);
}

static inline bool cpu_fifo_is_writable(void)
{
  return REG_GET_FIELD(SIO_FIFO_ST, SIO_FIFO_ST_RDY);
}

// --- Inter-core FIFO blocking operations ---

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

// --- Inter-core FIFO non-blocking operations ---

static inline bool cpu_fifo_try_read(uint32_t* value)
{
  if (!cpu_fifo_is_readable())
  {
    return false;
  }

  *value = *SIO_FIFO_RD;
  return true;
}

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

// --- Inter-core FIFO maintenance ---

static inline void cpu_fifo_flush(void)
{
  uint32_t dummy;
  while (cpu_fifo_try_read(&dummy))
  {
    __asm volatile("nop");
  }
  sev();
}

static inline bool cpu_fifo_write_echoed(uint32_t const* const buffer, size_t const len)
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
