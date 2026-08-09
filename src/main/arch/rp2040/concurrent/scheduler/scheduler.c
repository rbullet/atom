#include "internal.h"

static thread_t bootstrap_thread;
static thread_t idle_thread[CPU_COUNT];

static uint32_t idle_thread_stack[CPU_COUNT][IDLE_THREAD_STACK_SIZE];
static uint32_t deferred_task_thread_stack[DEFERRED_TASK_THREAD_STACK_SIZE];

static bool core1_started = false;

execution_context_t execution_context[CPU_COUNT] =
{
  {.spinlock = SPINLOCK_INITIALIZER, .ready_queue = LIST_INITIALIZER, .current_thread = NULL, .idle_thread = &idle_thread[0]},
  {.spinlock = SPINLOCK_INITIALIZER, .ready_queue = LIST_INITIALIZER, .current_thread = NULL, .idle_thread = &idle_thread[1]},
};

static void scheduler_thread_terminate_current(void* retval);

static __attribute__((noreturn)) void* idle_thread_manager(__attribute__((unused)) void* const arg)
{
#ifdef DEBUG
  ATOM_ASSERT(interrupts_are_enabled(), "Interrupts must be enabled in idle thread");
#endif
  for (;;)
  {
    wfi();
  }
}

static void scheduler_thread_init_common(thread_t* const thread, uint32_t* const stack_base, size_t const stack_size)
{
  thread->stack_base = stack_base;
  thread->stack_size = stack_size;
  thread->state = THREAD_NEW;
  thread->state_lock = SPINLOCK_INITIALIZER;
  thread->scheduler_node = LIST_NODE_INITIALIZER;
  thread->waiters = LIST_INITIALIZER;
  thread->waiters_spinlock = SPINLOCK_INITIALIZER;
  thread->context.type = THREAD_CONTEXT_NONE;
}

static void scheduler_thread_init_bootstrap(thread_t* thread, uint32_t* stack_base, size_t const stack_size)
{
  scheduler_thread_init_common(thread, stack_base, stack_size);
  thread->sp = PTR_OFFSET(stack_base, 1, stack_size);
  thread->state = THREAD_RUNNING;
  thread->deadline = scheduler_timestamp_now();
  execution_context[CPUID].current_thread = thread;
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

void scheduler_thread_terminate_current(void* retval)
{
  thread_t* thread = scheduler_thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&thread->state_lock)
    {
      thread_terminated_context_init(&thread->context.terminated, retval);
    }
  }
  thread_process_event(thread, THREAD_EVENT_TERMINATE);
}

__attribute__((noreturn)) static void scheduler_enter_idle(void)
{
  idle_thread_manager(NULL); // Enter idle loop on core 1
}

void scheduler_init_hardware(void)
{
  WITH_INTERRUPTS_DISABLED
  {
    if (CPU_IS_CORE_0)
    {
      interrupts_set_handler(IRQ_SYS_TICK, scheduler_sys_tick_handler);
      interrupts_set_handler(IRQ_PEND_SV, scheduler_pendsv_handler);
    }

    // Configure SysTick for 1ms tick interval
    REG_SET_FIELD(SYST_CSR, PPB_SYST_CSR_ENABLE, 0); // Disable during setup
    REG_SET_FIELD(SYST_RVR, PPB_SYST_RVR_RELOAD, (CPU_FREQUENCY_HZ / 1000) - 1); // 1ms tick
    REG_SET_FIELD(SYST_CVR, PPB_SYST_CVR_CURRENT, 0); // Clear current counter
    REG_SET_FIELD(NVIC_IPR3, PPB_NVIC_IPR3_IP_14, 3); // PendSV -> lowest priority
    REG_SET_FIELD(NVIC_IPR3, PPB_NVIC_IPR3_IP_15, 0); // Systick -> Highest priority
    REG_WRITE(SYST_CSR, SYST_CSR_ENABLE | SYST_CSR_TICKINT | SYST_CSR_CLKSOURCE);
  }
}

void scheduler_init(void)
{
  extern uint32_t const _sstack0;
  extern uint32_t const _estack0;
  if (CPU_IS_CORE_0)
  {
    scheduler_thread_init_bootstrap(&bootstrap_thread, (uint32_t*)&_sstack0, ((uintptr_t)&_estack0 - (uintptr_t)&_sstack0));

    scheduler_thread_init(&deferred_task_thread, deferred_task_thread_stack, DEFERRED_TASK_THREAD_STACK_SIZE, scheduler_deferred_task_callback, NULL);
    thread_process_event(&deferred_task_thread, THREAD_EVENT_START);

    scheduler_thread_init(&idle_thread[CPUID], idle_thread_stack[CPUID], IDLE_THREAD_STACK_SIZE, idle_thread_manager, NULL);
    thread_process_event(&idle_thread[CPUID], THREAD_EVENT_START);
  }
  else
  {
    scheduler_thread_init_bootstrap(&idle_thread[CPUID], idle_thread_stack[CPUID], IDLE_THREAD_STACK_SIZE);
    __asm__ volatile("MSR PSP, %0" :: "r"(execution_context[CPUID].current_thread->sp) : "memory");
    cpu_stack_set_mode(STACK_MODE_PSP);
  }
  scheduler_init_hardware();
  if (CPU_IS_CORE_1)
  {
    scheduler_enter_idle();
  }
}

// --- Boot Core 1 ---
void scheduler_start_secondary(void)
{
#ifdef DEBUG
  ATOM_ASSERT(!core1_started, "Secondary scheduler already started");
#endif

  extern uint32_t const _emsp1; // End of MSP for core 1
  extern uint32_t const relocated_vector_table;

  uint32_t const release_sequence[] = {
    0,
    0,
    1,
    (uint32_t)&relocated_vector_table,
    (uint32_t)&_emsp1,
    (uint32_t)scheduler_init
  };

  while (!cpu_fifo_write_echoed(release_sequence, ARRAY_SIZE(release_sequence)))
  {
    __asm__ volatile("nop");
  }

  core1_started = true;
}
