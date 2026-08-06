#include <stdbool.h>
#include <stddef.h>

#include <atom.h>
#include "rp2040/concurrent/hardware_spinlock.h"

// --- SIO peripheral registers (from RP2040 SVD) ---
#define SIO_BASE 0XD0000000
#define SIO_SPINLOCK0_OFFSET 0X0100
#define SIO_SPINLOCK1_OFFSET 0X0104
#define SIO_SPINLOCK2_OFFSET 0X0108
#define SIO_SPINLOCK3_OFFSET 0X010C
#define SIO_SPINLOCK4_OFFSET 0X0110
#define SIO_SPINLOCK5_OFFSET 0X0114
#define SIO_SPINLOCK6_OFFSET 0X0118
#define SIO_SPINLOCK7_OFFSET 0X011C
#define SIO_SPINLOCK8_OFFSET 0X0120
#define SIO_SPINLOCK9_OFFSET 0X0124
#define SIO_SPINLOCK10_OFFSET 0X0128
#define SIO_SPINLOCK11_OFFSET 0X012C
#define SIO_SPINLOCK12_OFFSET 0X0130
#define SIO_SPINLOCK13_OFFSET 0X0134
#define SIO_SPINLOCK14_OFFSET 0X0138
#define SIO_SPINLOCK15_OFFSET 0X013C
#define SIO_SPINLOCK16_OFFSET 0X0140
#define SIO_SPINLOCK17_OFFSET 0X0144
#define SIO_SPINLOCK18_OFFSET 0X0148
#define SIO_SPINLOCK19_OFFSET 0X014C
#define SIO_SPINLOCK20_OFFSET 0X0150
#define SIO_SPINLOCK21_OFFSET 0X0154
#define SIO_SPINLOCK22_OFFSET 0X0158
#define SIO_SPINLOCK23_OFFSET 0X015C
#define SIO_SPINLOCK24_OFFSET 0X0160
#define SIO_SPINLOCK25_OFFSET 0X0164
#define SIO_SPINLOCK26_OFFSET 0X0168
#define SIO_SPINLOCK27_OFFSET 0X016C
#define SIO_SPINLOCK28_OFFSET 0X0170
#define SIO_SPINLOCK29_OFFSET 0X0174
#define SIO_SPINLOCK30_OFFSET 0X0178
#define SIO_SPINLOCK31_OFFSET 0X017C

hardware_spinlock_t* const hardware_spinlock0 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK0_OFFSET));
hardware_spinlock_t* const hardware_spinlock1 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK1_OFFSET));
hardware_spinlock_t* const hardware_spinlock2 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK2_OFFSET));
hardware_spinlock_t* const hardware_spinlock3 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK3_OFFSET));
hardware_spinlock_t* const hardware_spinlock4 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK4_OFFSET));
hardware_spinlock_t* const hardware_spinlock5 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK5_OFFSET));
hardware_spinlock_t* const hardware_spinlock6 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK6_OFFSET));
hardware_spinlock_t* const hardware_spinlock7 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK7_OFFSET));
hardware_spinlock_t* const hardware_spinlock8 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK8_OFFSET));
hardware_spinlock_t* const hardware_spinlock9 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK9_OFFSET));
hardware_spinlock_t* const hardware_spinlock10 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK10_OFFSET));
hardware_spinlock_t* const hardware_spinlock11 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK11_OFFSET));
hardware_spinlock_t* const hardware_spinlock12 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK12_OFFSET));
hardware_spinlock_t* const hardware_spinlock13 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK13_OFFSET));
hardware_spinlock_t* const hardware_spinlock14 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK14_OFFSET));
hardware_spinlock_t* const hardware_spinlock15 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK15_OFFSET));
hardware_spinlock_t* const hardware_spinlock16 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK16_OFFSET));
hardware_spinlock_t* const hardware_spinlock17 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK17_OFFSET));
hardware_spinlock_t* const hardware_spinlock18 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK18_OFFSET));
hardware_spinlock_t* const hardware_spinlock19 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK19_OFFSET));
hardware_spinlock_t* const hardware_spinlock20 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK20_OFFSET));
hardware_spinlock_t* const hardware_spinlock21 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK21_OFFSET));
hardware_spinlock_t* const hardware_spinlock22 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK22_OFFSET));
hardware_spinlock_t* const hardware_spinlock23 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK23_OFFSET));
hardware_spinlock_t* const hardware_spinlock24 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK24_OFFSET));
hardware_spinlock_t* const hardware_spinlock25 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK25_OFFSET));
hardware_spinlock_t* const hardware_spinlock26 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK26_OFFSET));
hardware_spinlock_t* const hardware_spinlock27 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK27_OFFSET));
hardware_spinlock_t* const hardware_spinlock28 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK28_OFFSET));
hardware_spinlock_t* const hardware_spinlock29 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK29_OFFSET));
hardware_spinlock_t* const hardware_spinlock30 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK30_OFFSET));
hardware_spinlock_t* const hardware_spinlock31 = ((hardware_spinlock_t*)(SIO_BASE + SIO_SPINLOCK31_OFFSET));

void hardware_spinlock_init(void)
{
  for (size_t i = 0; i < RP2040_SPINLOCK_COUNT; i++)
  {
    hardware_spinlock0[i] = 1; // Clear RP2040 hardware spinlock state.
  }
}

bool hardware_spinlock_try_lock(hardware_spinlock_t* const hardware_spinlock)
{
  __asm volatile ("dmb" ::: "memory");
  bool acquired = (*hardware_spinlock != 0);
  __asm volatile ("dmb" ::: "memory");
  return acquired;
}

void hardware_spinlock_lock(hardware_spinlock_t* const hardware_spinlock)
{
  __asm volatile ("dmb" ::: "memory");
  while (!*hardware_spinlock)
  {
    __asm volatile("nop");
  }
  __asm volatile ("dmb" ::: "memory");
}

void hardware_spinlock_unlock(hardware_spinlock_t* const hardware_spinlock)
{
  __asm volatile("dmb" ::: "memory");
  *hardware_spinlock = 0;
  __asm volatile("dmb" ::: "memory");
}
