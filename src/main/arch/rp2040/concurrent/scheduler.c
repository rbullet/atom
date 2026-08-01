#include <string.h>

#include "rp2040/atom.h"
#include "rp2040/concurrent/deferred_task.h"
#include "rp2040/concurrent/interrupts.h"
#include "rp2040/concurrent/scheduler.h"
#include "rp2040/concurrent/spinlock.h"

// --- PPB peripheral registers (from RP2040 SVD) ---
#define PPB_BASE 0XE0000000
#define PPB_ICSR_OFFSET 0XED04
#define PPB_SYST_CSR_OFFSET 0XE010
#define PPB_SYST_CSR_ENABLE_MASK 0X1
#define PPB_SYST_CSR_ENABLE_OFFSET 0
#define PPB_SYST_RVR_OFFSET 0XE014
#define PPB_SYST_RVR_RELOAD_MASK 0XFFFFFF
#define PPB_SYST_RVR_RELOAD_OFFSET 0
#define PPB_SYST_CVR_OFFSET 0XE018
#define PPB_SYST_CVR_CURRENT_MASK 0XFFFFFF
#define PPB_SYST_CVR_CURRENT_OFFSET 0
#define PPB_SYST_CALIB_OFFSET 0XE01C
#define PPB_NVIC_IPR3_OFFSET 0XE40C
#define PPB_NVIC_IPR3_IP_14_MASK 0XC00000
#define PPB_NVIC_IPR3_IP_14_OFFSET 22
#define PPB_NVIC_IPR3_IP_15_MASK 0XC0000000
#define PPB_NVIC_IPR3_IP_15_OFFSET 30

// --- Cortex-M SysTick & PendSV Registers ---
#define ICSR       REG(PPB_BASE, PPB_ICSR_OFFSET)
#define ICSR_PENDSVSET (1u << 28)

#define SYST_CSR   REG(PPB_BASE, PPB_SYST_CSR_OFFSET)
#define SYST_RVR   REG(PPB_BASE, PPB_SYST_RVR_OFFSET)
#define SYST_CVR   REG(PPB_BASE, PPB_SYST_CVR_OFFSET)
#define SYST_CALIB REG(PPB_BASE, PPB_SYST_CALIB_OFFSET)

#define NVIC_IPR3 REG(PPB_BASE, PPB_NVIC_IPR3_OFFSET)

#define SYST_CSR_ENABLE    (1u << 0)
#define SYST_CSR_TICKINT   (1u << 1)
#define SYST_CSR_CLKSOURCE (1u << 2)

// --- Scheduler Configuration ---
#define XPSR_THREAD 0x01000000            // Default xPSR value for new threads
#define IDLE_THREAD_STACK_SIZE 1024       // Idle thread stack size in bytes
#define DEFERRED_TASK_THREAD_STACK_SIZE 1024      // Deferred thread stack size in bytes

#define THREAD_STACK_CANARY 0xDEADBEEF

spinlock_t* scheduler_spinlock = NULL;
spinlock_t* thread_spinlock = NULL;
spinlock_t* deferred_tasks_spinlock = NULL;

static duration_t const scheduler_task_quantum = DURATION_INITIALIZER(10, MILLISECONDS);

// --- External symbols from linker script ---
extern uint32_t const _sstack0;
extern uint32_t const _estack0;
extern uint32_t const _emsp1; // End of MSP for core 1
extern uint32_t const relocated_vector_table;

static thread_t bootstrap_thread;

// --- Current thread per core ---
static thread_t* volatile current_thread[CPU_COUNT];

// --- Idle threads ---
static uint32_t idle_thread_stack[CPU_COUNT][IDLE_THREAD_STACK_SIZE / sizeof(uint32_t)];
static thread_t idle_thread[CPU_COUNT];

// --- Timer threads ---
static uint32_t deferred_task_thread_stack[DEFERRED_TASK_THREAD_STACK_SIZE / sizeof(uint32_t)];
static thread_t deferred_task_thread;

// --- Scheduler ticks ---
static volatile uint64_t scheduler_tick = 0;

// --- Forward declarations ---
static __attribute__((noreturn)) void* idle_thread_manager(void* arg);
static __attribute__((noreturn)) void* deferred_task_manager(void* arg);
static void scheduler_sys_tick_handler(void);
static void scheduler_pend_sv_handler(void);
static void scheduler_boot_core_1(void);
static bool scheduler_deferred_task_expiration_comparator(list_node_t const* left, list_node_t const* right);
static void scheduler_thread_transition_to_running(thread_t* thread);
static __attribute__((noreturn)) void scheduler_thread_terminate_current(void* retval);
void scheduler_request_context_switch(void);

// --- Thread Queues ---
static list_t ready_queue = LIST_INITIALIZER;
static sorted_list_t deferred_tasks_queue = SORTED_LIST_INITIALIZER(scheduler_deferred_task_expiration_comparator);

// --- SysTick Handler ---
// Handles scheduler tick increase the sys tick, handles the deferred tasks and quantum expiration
void scheduler_sys_tick_handler(void)
{
  WITH_INTERRUPTS_DISABLED
  {
    if (CPU_IS_CORE_0)
    {
      scheduler_tick++;
      WITH_SPINLOCK(deferred_tasks_spinlock)
      {
        if (!sorted_list_is_empty(&deferred_tasks_queue))
        {
          deferred_task_t* deferred_task = CONTAINER_OF(deferred_tasks_queue.head, deferred_task_t, scheduler_node);
          if (scheduler_timestamp_is_expired(deferred_task->deadline))
          {
            scheduler_thread_resume(&deferred_task_thread);
          }
        }
      }
    }
  }
  WITH_SPINLOCK(scheduler_spinlock)
  {
    if (!list_is_empty(&ready_queue) && (scheduler_timestamp_is_expired(current_thread[CPUID]->deadline) || (current_thread[CPUID] == &idle_thread[CPUID])))
    {
      scheduler_request_context_switch();
    }
  }
}

__attribute__((used)) static void scheduler_save_current_sp(uint32_t* sp)
{
  // current_thread[CPUID] still points to the old thread here
  current_thread[CPUID]->sp = sp;
}

__attribute__((used)) static thread_t* scheduler_pick_next_thread(void)
{
  thread_t* next = NULL;
  WITH_SPINLOCK(scheduler_spinlock)
  {
    next = list_is_empty(&ready_queue) ? &idle_thread[CPUID] : CONTAINER_OF(list_pop(&ready_queue), thread_t, scheduler_node);
  }
  return next;
}

__attribute__((used)) static uintptr_t scheduler_switch_to(thread_t* next)
{
  thread_t* current = current_thread[CPUID];
  ATOM_ASSERT(*current->stack_base == THREAD_STACK_CANARY, "Stack overflow detected in thread");
  if (current->state == THREAD_RUNNING && current != &idle_thread[CPUID])
  {
    scheduler_thread_return_current_to_queue();
  }
  scheduler_thread_transition_to_running(next);
  return (uintptr_t)&next->sp;
}

// --- PendSV Handler ---
// Saves the current thread context and restores the next scheduled thread.
__attribute__((naked)) void scheduler_pend_sv_handler(void)
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

void scheduler_request_context_switch(void)
{
  __asm volatile("dmb ish" ::: "memory");
  ICSR = ICSR_PENDSVSET;
}


static __attribute__((noreturn)) void* idle_thread_manager(__attribute__((unused)) void* const arg)
{
  for (;;)
  {
    wfi();
  }
}

static void scheduler_thread_init_common(thread_t* thread, uint32_t* stack_base, size_t stack_size)
{
  thread->stack_base = stack_base;
  thread->stack_size = stack_size;
  *thread->stack_base = THREAD_STACK_CANARY;
  thread->state = THREAD_READY;
  thread->scheduler_node = LIST_NODE_INITIALIZER;
  thread->waiters = LIST_INITIALIZER;
  thread->mutex = MUTEX_INITIALIZER;
  thread->completion = CONDITION_VARIABLE_INITIALIZER;
  thread->retval = NULL;
}

static void scheduler_thread_init_bootstrap(thread_t* thread, uint32_t* stack_base, size_t const stack_size)
{
  scheduler_thread_init_common(thread, stack_base, stack_size);
  thread->sp = PTR_OFFSET(stack_base, 1, stack_size);
  thread->state = THREAD_RUNNING;
  thread->deadline = scheduler_timestamp_now();
  current_thread[CPUID] = thread;
}

// --- Thread Initialisation ---
void scheduler_thread_init(thread_t* thread, uint32_t* stack_base, size_t const stack_size, thread_func_t const start_routine, void* arg)
{
  scheduler_thread_init_common(thread, stack_base, stack_size);
  // Initialize thread stack and context
  uint32_t* sp = PTR_OFFSET(stack_base, 1, stack_size);
  /* Align stack pointer to 8 bytes */
  sp = (uint32_t*)((uintptr_t)sp & ~((uintptr_t)0x7));

  // Push initial CPU context on stack
  *(--sp) = XPSR_THREAD; // xPSR
  *(--sp) = (uint32_t)start_routine; // PC
  *(--sp) = (uint32_t)&scheduler_thread_terminate_current; // LR
  *(--sp) = 12; // R12
  *(--sp) = 3; // R3
  *(--sp) = 2; // R2
  *(--sp) = 1; // R1
  *(--sp) = (uint32_t)arg; // R0
  *(--sp) = 4; // R4
  *(--sp) = 5; // R5
  *(--sp) = 6; // R6
  *(--sp) = 7; // R7
  *(--sp) = 8; // R8
  *(--sp) = 9; // R9
  *(--sp) = 10; // R10
  *(--sp) = 11; // R11
  thread->sp = sp;
}

static inline void scheduler_assert_running_thread(thread_t const* thread)
{
  ATOM_ASSERT(thread->state == THREAD_RUNNING, "Thread must be running on this CPU");
  ATOM_ASSERT(thread == current_thread[CPUID], "Only current thread can perform this transition");
}

static inline thread_t* scheduler_current_running_thread(void)
{
  thread_t* thread = current_thread[CPUID];
  scheduler_assert_running_thread(thread);
  return thread;
}

static void scheduler_thread_transition_to_running(thread_t* thread)
{
  thread->deadline = scheduler_timestamp_add(scheduler_timestamp_now(), scheduler_task_quantum);
  thread->state = THREAD_RUNNING;
  current_thread[CPUID] = thread;
}

thread_t* scheduler_thread_current(void)
{
  return current_thread[CPUID];
}

void scheduler_thread_start(thread_t* thread)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(scheduler_spinlock)
    {
      ATOM_ASSERT(thread->state == THREAD_READY, "Only ready threads can be started");
      if (thread != &idle_thread[CPUID])
      {
        list_push(&ready_queue, &thread->scheduler_node);
      }
    }
  }
}

void scheduler_thread_resume(thread_t* thread)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(scheduler_spinlock)
    {
      switch (thread->state)
      {
      case THREAD_BLOCKED:
      case THREAD_SLEEPING:
        thread->state = THREAD_READY;
        list_push(&ready_queue, &thread->scheduler_node);
        break;
      case THREAD_RUNNING:
      case THREAD_READY:
        break;
      default:
        ATOM_ASSERT(false, "Invalid transition to READY");
        break;
      }
    }
  }
}

void scheduler_thread_return_current_to_queue()
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(scheduler_spinlock)
    {
      thread_t* thread = scheduler_current_running_thread();
      thread->state = THREAD_READY;
      list_push(&ready_queue, &thread->scheduler_node);
    }
  }
}

void scheduler_thread_block_current()
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(scheduler_spinlock)
    {
      thread_t* thread = scheduler_current_running_thread();
      thread->state = THREAD_BLOCKED;
    }
    scheduler_thread_yield_current();
  }
}

void scheduler_thread_block_current_on(list_t* wait_queue, spinlock_t* guarded_by)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(scheduler_spinlock)
    {
      thread_t* thread = scheduler_current_running_thread();
      thread->state = THREAD_BLOCKED;
      list_push(wait_queue, &thread->scheduler_node);
    }
    spinlock_unlock(guarded_by);
    scheduler_thread_yield_current();
  }
}

void scheduler_thread_yield_current()
{
  WITH_INTERRUPTS_ENABLED
  {
    scheduler_request_context_switch();
    wfi();
  }
}

static void scheduler_thread_wakeup_callback(void* arg)
{
  thread_t* thread = arg;
  scheduler_thread_resume(thread);
}

void scheduler_thread_sleep_current(duration_t const duration)
{
  WITH_INTERRUPTS_DISABLED
  {
    thread_t* thread = scheduler_current_running_thread();
    WITH_SPINLOCK(scheduler_spinlock)
    {
      thread->state = THREAD_SLEEPING;
    }
    deferred_task_t wakeup_task;
    deferred_task_start_after(&wakeup_task, duration, scheduler_thread_wakeup_callback, thread);
    scheduler_thread_yield_current();
  }
}

void scheduler_thread_terminate_current(void* retval)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(thread_spinlock)
    {
      thread_t* thread = scheduler_current_running_thread();
      thread->retval = retval;
      thread->state = THREAD_TERMINATED;

      while (!list_is_empty(&thread->waiters))
      {
        thread_t* const waiter = CONTAINER_OF(list_pop(&thread->waiters), thread_t, scheduler_node);
        scheduler_thread_resume(waiter);
      }
    }
    scheduler_thread_yield_current();
  }
  // should never return
  for (;;);
}

void* scheduler_thread_join(thread_t* thread)
{
  WITH_INTERRUPTS_DISABLED
  {
    spinlock_lock(thread_spinlock);
    if (thread->state == THREAD_TERMINATED)
    {
      spinlock_unlock(thread_spinlock);
      return thread->retval;
    }
    scheduler_thread_block_current_on(&thread->waiters, thread_spinlock);
  }
  return thread->retval;
}

void scheduler_task_schedule(deferred_task_t* deferred_task)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_MUTEX(&deferred_task->mutex)
    {
      if (deferred_task->state != DEFERRED_TASK_IDLE)
      {
        return;
      }
      deferred_task->scheduler_node = LIST_NODE_INITIALIZER;
      deferred_task->state = DEFERRED_TASK_SCHEDULED;
      deferred_task->deadline = scheduler_timestamp_add(scheduler_timestamp_now(), deferred_task->initial_delay);
      WITH_SPINLOCK(deferred_tasks_spinlock)
      {
        ATOM_ASSERT(deferred_task->scheduler_node.next == NULL && deferred_task->scheduler_node.previous == NULL, "Deferred task already linked");
        sorted_list_add(&deferred_tasks_queue, &deferred_task->scheduler_node);
      }
    }
  }
}

void scheduler_task_cancel(deferred_task_t* deferred_task)
{
  WITH_INTERRUPTS_DISABLED
  {
    WITH_MUTEX(&deferred_task->mutex)
    {
      switch (deferred_task->state)
      {
      case DEFERRED_TASK_SCHEDULED:
        deferred_task->state = DEFERRED_TASK_CANCELLED;
        break;

      case DEFERRED_TASK_RUNNING:
        deferred_task->state = DEFERRED_TASK_CANCELLED;
        if (current_thread[CPUID] != &deferred_task_thread)
        {
          condition_variable_wait(&deferred_task->completion, &deferred_task->mutex);
        }
        break;

      default:
        break;
      }
    }
  }
}

static __attribute__((noreturn)) void* deferred_task_manager(void* const arg)
{
  (void)arg;
  for (;;)
  {
    thread_wait();
    list_t expired_tasks = LIST_INITIALIZER;
    WITH_INTERRUPTS_DISABLED
    {
      WITH_SPINLOCK(deferred_tasks_spinlock)
      {
        while (!sorted_list_is_empty(&deferred_tasks_queue))
        {
          deferred_task_t* deferred_task = CONTAINER_OF(deferred_tasks_queue.head, deferred_task_t, scheduler_node);
          if (!scheduler_timestamp_is_expired(deferred_task->deadline))
          {
            break;
          }
          sorted_list_remove(&deferred_tasks_queue, &deferred_task->scheduler_node);
          list_push(&expired_tasks, &deferred_task->scheduler_node);
        }
      }
    }
    while (!list_is_empty(&expired_tasks))
    {
      deferred_task_t* const deferred_task = CONTAINER_OF(list_pop(&expired_tasks), deferred_task_t, scheduler_node);
      WITH_MUTEX(&deferred_task->mutex)
      {
        if (deferred_task->state != DEFERRED_TASK_SCHEDULED)
        {
          continue;
        }
        deferred_task->state = DEFERRED_TASK_RUNNING;
      }
      deferred_task->callback(deferred_task->arg);
      WITH_MUTEX(&deferred_task->mutex)
      {
        if (deferred_task->state == DEFERRED_TASK_CANCELLED)
        {
          condition_variable_broadcast(&deferred_task->completion);
        }
        else if (deferred_task_is_periodic(deferred_task))
        {
          WITH_SPINLOCK(deferred_tasks_spinlock)
          {
            deferred_task->state = DEFERRED_TASK_SCHEDULED;
            deferred_task->deadline = scheduler_timestamp_add(deferred_task->deadline, deferred_task->period);
            sorted_list_add(&deferred_tasks_queue, &deferred_task->scheduler_node);
          }
        }
        else
        {
          deferred_task->state = DEFERRED_TASK_COMPLETED;
          condition_variable_broadcast(&deferred_task->completion);
        }
      }
    }
  }
}

timestamp_t scheduler_timestamp_now(void)
{
  return (timestamp_t){.value = scheduler_tick};
}

timestamp_t scheduler_timestamp_add(timestamp_t const timestamp, duration_t const duration)
{
  return (timestamp_t){.value = timestamp.value + (uint64_t)duration_convert_to(duration, MILLISECONDS).value};
}

bool scheduler_timestamp_is_before(timestamp_t const first, timestamp_t const second)
{
  return (int64_t)(first.value - second.value) < 0;
}

bool scheduler_timestamp_is_before_or_equal(timestamp_t const first, timestamp_t const second)
{
  return (int64_t)(first.value - second.value) <= 0;
}

bool scheduler_timestamp_is_after(timestamp_t const first, timestamp_t const second)
{
  return (int64_t)(first.value - second.value) > 0;
}

bool scheduler_timestamp_is_after_or_equal(timestamp_t const first, timestamp_t const second)
{
  return (int64_t)(first.value - second.value) >= 0;
}

bool scheduler_timestamp_is_expired(timestamp_t const deadline)
{
  return (int64_t)(scheduler_tick - deadline.value) >= 0;
}

duration_t scheduler_timestamp_duration_since(timestamp_t const timestamp)
{
  return duration_of((float)(scheduler_tick - timestamp.value), MILLISECONDS);
}

static bool scheduler_deferred_task_expiration_comparator(list_node_t const* left, list_node_t const* right)
{
  deferred_task_t const* first = CONTAINER_OF(left, deferred_task_t, scheduler_node);
  deferred_task_t const* second = CONTAINER_OF(right, deferred_task_t, scheduler_node);
  return scheduler_timestamp_is_before(first->deadline, second->deadline);
}

static void scheduler_init_hardware(void)
{
  WITH_INTERRUPTS_DISABLED
  {
    if (CPU_IS_CORE_0)
    {
      interrupts_set_handler(IRQ_SYS_TICK, scheduler_sys_tick_handler);
      interrupts_set_handler(IRQ_PEND_SV, scheduler_pend_sv_handler);
    }

    // Configure SysTick for 1ms tick interval
    REG_SET_FIELD(SYST_CSR, PPB_SYST_CSR_ENABLE, 0); // Disable during setup
    REG_SET_FIELD(SYST_RVR, PPB_SYST_RVR_RELOAD, (CPU_FREQUENCY_HZ / 1000) - 1); // 1ms tick
    REG_SET_FIELD(SYST_CVR, PPB_SYST_CVR_CURRENT, 0); // Clear current counter
    REG_SET_FIELD(NVIC_IPR3, PPB_NVIC_IPR3_IP_14, 3); // PendSV -> lowest priority
    REG_SET_FIELD(NVIC_IPR3, PPB_NVIC_IPR3_IP_15, 0); // Systick -> Highest priority
    SYST_CSR = SYST_CSR_ENABLE | SYST_CSR_TICKINT | SYST_CSR_CLKSOURCE;
  }
}

__attribute__((noreturn)) static void scheduler_enter_idle(void)
{
  idle_thread_manager(NULL); // Enter idle loop on core 1
}

void scheduler_init(void)
{
  spinlock_pool_ensure_initialized(&scheduler_spinlock, SPINLOCK_EXCLUSIVE);
  spinlock_pool_ensure_initialized(&thread_spinlock, SPINLOCK_EXCLUSIVE);
  spinlock_pool_ensure_initialized(&deferred_tasks_spinlock, SPINLOCK_EXCLUSIVE);
  if (CPU_IS_CORE_0)
  {
    scheduler_thread_init_bootstrap(&bootstrap_thread, (uint32_t*)&_sstack0, ((uintptr_t)&_estack0 - (uintptr_t)&_sstack0));
    scheduler_thread_init(&deferred_task_thread, deferred_task_thread_stack, DEFERRED_TASK_THREAD_STACK_SIZE, deferred_task_manager, NULL);
    scheduler_thread_init(&idle_thread[CPUID], idle_thread_stack[CPUID], IDLE_THREAD_STACK_SIZE, idle_thread_manager, NULL);
  }
  else
  {
    scheduler_thread_init_bootstrap(&idle_thread[CPUID], idle_thread_stack[CPUID], IDLE_THREAD_STACK_SIZE);
    __asm__ volatile("MSR PSP, %0" :: "r"(current_thread[CPUID]->sp) : "memory");
    cpu_stack_set_mode(STACK_MODE_PSP);
  }
  scheduler_init_hardware();
  if (CPU_IS_CORE_0)
  {
    scheduler_thread_start(&deferred_task_thread);
    scheduler_boot_core_1();
  }
  else
  {
    scheduler_enter_idle();
  }
}

// --- Boot Core 1 ---
static void scheduler_boot_core_1(void)
{
  uint32_t const release_sequence[] = {0, 0, 1, (uint32_t)&relocated_vector_table, (uint32_t)&_emsp1, (uint32_t)scheduler_init};
  uint32_t const size = ARRAY_SIZE(release_sequence);
  while (!cpu_fifo_write_echoed(release_sequence, size));
}
