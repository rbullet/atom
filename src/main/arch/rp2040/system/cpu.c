#include "rp2040/atom.h"

void cpu_breakpoint(void)
{
  __asm__ volatile("bkpt #0");
}

inline uint32_t cpu_get_id(void)
{
  return CPUID;
}