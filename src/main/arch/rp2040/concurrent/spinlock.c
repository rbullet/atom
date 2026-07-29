#include <stdbool.h>
#include "concurrent/spinlock.h"

bool spinlock_try_lock(spinlock_t* spinlock)
{
  __asm volatile ("dmb" ::: "memory");
  bool acquired = (*spinlock != 0);
  __asm volatile ("dmb" ::: "memory");
  return acquired;
}

void spinlock_lock(spinlock_t* spinlock)
{
  __asm volatile ("dmb" ::: "memory");
  while (!*spinlock)
  {
    __asm volatile("nop");
  }
  __asm volatile ("dmb" ::: "memory");
}

void spinlock_unlock(spinlock_t* spinlock)
{
  __asm volatile("dmb" ::: "memory");
  *spinlock = 0;
  __asm volatile("dmb" ::: "memory");
}
