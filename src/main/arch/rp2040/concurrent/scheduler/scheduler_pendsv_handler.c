#include "internal.h"

static duration_t const scheduler_task_quantum = DURATION_INITIALIZER(10, MILLISECONDS);

__attribute__((used, unused)) static void scheduler_save_current_sp(uint32_t* sp)
{
  execution_context[CPUID].current_thread->sp = sp;
}

static inline thread_t* scheduler_pick_next_thread_on_core(uint32_t const cpuid)
{
  thread_t* next = NULL;
  WITH_SPINLOCK(&execution_context[cpuid].spinlock)
  {
    if (!list_is_empty(&execution_context[cpuid].ready_queue))
    {
      next = CONTAINER_OF(list_pop(&execution_context[cpuid].ready_queue), thread_t, scheduler_node);
    }
  }
  return next;
}

static inline thread_t* scheduler_pick_thread_on_current_core(void)
{
  return scheduler_pick_next_thread_on_core(CPUID);
}

static inline thread_t* scheduler_steal_thread_on_over_core(void)
{
  return scheduler_pick_next_thread_on_core((CPUID + 1) % CPU_COUNT);
}

__attribute__((used, unused)) static thread_t* scheduler_pick_next_thread(void)
{
  thread_t* next = scheduler_pick_thread_on_current_core();
  if (next == NULL)
  {
    next = scheduler_steal_thread_on_over_core();
  }
  return next != NULL ? next : execution_context[CPUID].idle_thread;
}

__attribute__((used, unused)) static uintptr_t scheduler_switch_to(thread_t* next)
{
  thread_t* previous = execution_context[CPUID].current_thread;

  execution_context[CPUID].current_thread = next;
  next->deadline = scheduler_timestamp_add(scheduler_timestamp_now(), scheduler_task_quantum);

  scheduler_thread_process_event(previous, THREAD_EVENT_YIELD);
  scheduler_thread_process_event(next, THREAD_EVENT_RUN);

  return (uintptr_t)&next->sp;
}

void scheduler_request_context_switch(void)
{
  __asm volatile("dmb ish" ::: "memory");
  REG_WRITE(ICSR, ICSR_PENDSVSET);
}

// --- PendSV Handler ---
// Saves the current thread context and restores the next scheduled thread.
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

