#include "rp2040/atom.h"

void cpu_breakpoint(void)
{
  __asm__ volatile("bkpt #0");
}
