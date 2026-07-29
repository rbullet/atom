#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "rp2040/base.h"
#include "rp2040/rp2040.h"
#include "util/helpers.h"

#define SIO_FIFO_ST     ((volatile uint32_t*)(SIO_BASE + SIO_FIFO_ST_OFFSET))
#define SIO_FIFO_RD     ((volatile uint32_t*)(SIO_BASE + SIO_FIFO_RD_OFFSET))
#define SIO_FIFO_WR     ((volatile uint32_t*)(SIO_BASE + SIO_FIFO_WR_OFFSET))

#define CPU_COUNT 2
#define CPUID           (*((volatile uint32_t*)(SIO_BASE + SIO_CPUID_OFFSET)))
#define CPU_IS_CORE_0   (CPUID == 0)   // Check if running on core 0
#define CPU_IS_CORE_1   (CPUID == 1)   // Check if running on core 1

// --- CPU FIFO status helpers ---
static inline bool cpu_fifo_is_readable(void)
{
  return REG_GET_FIELD(*SIO_FIFO_ST, SIO_FIFO_ST_VLD);
}

static inline bool cpu_fifo_is_writable(void)
{
  return REG_GET_FIELD(*SIO_FIFO_ST, SIO_FIFO_ST_RDY);
}

// --- CPU FIFO blocking read ---
static inline uint32_t cpu_fifo_read(void)
{
  while (!cpu_fifo_is_readable())
  {
    wfe();
  }
  return *SIO_FIFO_RD;
}

// --- CPU FIFO blocking write ---
static inline void cpu_fifo_write(uint32_t value)
{
  while (!cpu_fifo_is_writable())
  {
    wfe();
  }
  *SIO_FIFO_WR = value;
  sev(); // Wake other core
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
static inline bool cpu_fifo_try_write(uint32_t value)
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

#ifdef __cplusplus
}
#endif
