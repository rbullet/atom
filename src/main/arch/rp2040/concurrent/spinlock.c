#include <stdbool.h>
#include "rp2040/concurrent/spinlock.h"

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

spinlock_t* const spinlock0 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK0_OFFSET));
spinlock_t* const spinlock1 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK1_OFFSET));
spinlock_t* const spinlock2 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK2_OFFSET));
spinlock_t* const spinlock3 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK3_OFFSET));
spinlock_t* const spinlock4 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK4_OFFSET));
spinlock_t* const spinlock5 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK5_OFFSET));
spinlock_t* const spinlock6 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK6_OFFSET));
spinlock_t* const spinlock7 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK7_OFFSET));
spinlock_t* const spinlock8 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK8_OFFSET));
spinlock_t* const spinlock9 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK9_OFFSET));
spinlock_t* const spinlock10 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK10_OFFSET));
spinlock_t* const spinlock11 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK11_OFFSET));
spinlock_t* const spinlock12 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK12_OFFSET));
spinlock_t* const spinlock13 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK13_OFFSET));
spinlock_t* const spinlock14 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK14_OFFSET));
spinlock_t* const spinlock15 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK15_OFFSET));
spinlock_t* const spinlock16 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK16_OFFSET));
spinlock_t* const spinlock17 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK17_OFFSET));
spinlock_t* const spinlock18 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK18_OFFSET));
spinlock_t* const spinlock19 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK19_OFFSET));
spinlock_t* const spinlock20 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK20_OFFSET));
spinlock_t* const spinlock21 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK21_OFFSET));
spinlock_t* const spinlock22 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK22_OFFSET));
spinlock_t* const spinlock23 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK23_OFFSET));
spinlock_t* const spinlock24 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK24_OFFSET));
spinlock_t* const spinlock25 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK25_OFFSET));
spinlock_t* const spinlock26 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK26_OFFSET));
spinlock_t* const spinlock27 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK27_OFFSET));
spinlock_t* const spinlock28 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK28_OFFSET));
spinlock_t* const spinlock29 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK29_OFFSET));
spinlock_t* const spinlock30 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK30_OFFSET));
spinlock_t* const spinlock31 = ((spinlock_t*)(SIO_BASE + SIO_SPINLOCK31_OFFSET));

bool spinlock_try_lock(spinlock_t* const spinlock)
{
  __asm volatile ("dmb" ::: "memory");
  bool acquired = (*spinlock != 0);
  __asm volatile ("dmb" ::: "memory");
  return acquired;
}

void spinlock_lock(spinlock_t* const spinlock)
{
  __asm volatile ("dmb" ::: "memory");
  while (!*spinlock)
  {
    __asm volatile("nop");
  }
  __asm volatile ("dmb" ::: "memory");
}

void spinlock_unlock(spinlock_t* const spinlock)
{
  __asm volatile("dmb" ::: "memory");
  *spinlock = 0;
  __asm volatile("dmb" ::: "memory");
}
