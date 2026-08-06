#include <stdbool.h>

#include <atom.h>
#include "rp2040/concurrent/spinlock.h"
#include "rp2040/concurrent/hardware_spinlock.h"

#define SPINLOCK_GLOBAL_LOCK hardware_spinlock31

bool spinlock_try_lock(spinlock_t *const spinlock)
{
  bool acquired = false;

  WITH_HARDWARE_SPINLOCK(SPINLOCK_GLOBAL_LOCK)
  {
    if (spinlock->locked == 0)
    {
      spinlock->locked = 1;
      acquired = true;
    }
  }

  return acquired;
}

void spinlock_lock(spinlock_t *const spinlock)
{
  while (!spinlock_try_lock(spinlock))
  {
    __asm volatile("nop");
  }
}

void spinlock_unlock(spinlock_t *const spinlock)
{
  hardware_spinlock_lock(SPINLOCK_GLOBAL_LOCK);

  spinlock->locked = 0;

  hardware_spinlock_unlock(SPINLOCK_GLOBAL_LOCK);
}