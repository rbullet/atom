#include "internal.h"

static bool scheduler_in_pendsv(void)
{
  uint32_t ipsr;
  __asm volatile ("mrs %0, ipsr" : "=r"(ipsr));
  return ipsr == 14;
}

void scheduler_yield(void)
{
  if (scheduler_in_pendsv())
  {
    return;
  }
  __asm volatile("dmb ish" ::: "memory");
  REG_WRITE(ICSR, ICSR_PENDSVSET);

  if (!in_interrupt())
  {
    WITH_INTERRUPTS_ENABLED
    {
      wfi();
    }
  }
}

__attribute__((naked)) void scheduler_pendsv_handler(void)
{
  __asm volatile(
    "cpsid   i                          \n" // disable interrupts
    "mrs r0, psp                        \n"
    // --- Save low registers r4-r7 ---
    "sub     r0, r0, #32                \n"
    "str r4, [r0,#0]                    \n"
    "str r5, [r0,#4]                    \n"
    "str r6, [r0,#8]                    \n"
    "str r7, [r0,#12]                   \n"
    // --- Move high regs to low regs and save r8-r11 ---
    "mov r4, r8                         \n"
    "mov r5, r9                         \n"
    "mov r6, r10                        \n"
    "mov r7, r11                        \n"
    "str r4, [r0,#16]                   \n"
    "str r5, [r0,#20]                   \n"
    "str r6, [r0,#24]                   \n"
    "str r7, [r0,#28]                   \n"
  );
  __asm volatile(
    // --- Call scheduler functions ---
    "bl      scheduler_save_current_sp  \n"
    "bl      scheduler_pick_next_thread \n"
    "bl      scheduler_switch_to        \n"
  );
  __asm volatile(
    // --- Load PSP of next thread ---
    "ldr     r0, [r0]                   \n"
    // --- Restore high registers r8-r11 first (r4-r7 used only as scratch) ---
    "ldr r4, [r0,#16]                   \n"
    "ldr r5, [r0,#20]                   \n"
    "ldr r6, [r0,#24]                   \n"
    "ldr r7, [r0,#28]                   \n"
    "mov r8, r4                         \n"
    "mov r9, r5                         \n"
    "mov r10, r6                        \n"
    "mov r11, r7                        \n"
    // --- Restore low registers r4-r7 last so they keep their real values ---
    "ldr r4, [r0,#0]                    \n"
    "ldr r5, [r0,#4]                    \n"
    "ldr r6, [r0,#8]                    \n"
    "ldr r7, [r0,#12]                   \n"
    // --- Update PSP ---
    "add r0, r0, #32                    \n"
    "msr psp, r0                        \n"
    // --- Return from exception ---
    "ldr     r0, =0xFFFFFFFD            \n"
    "mov     lr, r0                     \n"
    "cpsie   i                          \n"
    "bx      lr                         \n"
  );
}

