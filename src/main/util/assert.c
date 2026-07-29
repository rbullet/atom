#include "util/assert.h"
#include "util/log.h"

void atom_assert(char const* file, int line, char const* message)
{
  log_fatal("Assertion failed: %s (%s:%d)", message, file, line);
  __asm volatile("bkpt #0");
  for (;;);
}
