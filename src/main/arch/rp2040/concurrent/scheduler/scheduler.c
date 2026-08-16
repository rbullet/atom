#include "internal.h"

// --- Internal state ---

static thread_t bootstrap_thread;
static thread_t idle_thread[CORE_COUNT];
static thread_t deferred_task_thread;

static uint32_t idle_thread_stack[CORE_COUNT][IDLE_THREAD_STACK_SIZE];
static uint32_t deferred_task_thread_stack[DEFERRED_TASK_THREAD_STACK_SIZE];

static duration_t const scheduler_task_quantum = DURATION_INITIALIZER(10, MILLISECONDS);

static bool core1_started = false;

// --- Per-core execution context ---

execution_context_t execution_context[CORE_COUNT] =
{
  {.spinlock = SPINLOCK_INITIALIZER, .ready_queue = LIST_INITIALIZER, .current_thread = NULL, .idle_thread = &idle_thread[0]},
  {.spinlock = SPINLOCK_INITIALIZER, .ready_queue = LIST_INITIALIZER, .current_thread = NULL, .idle_thread = &idle_thread[1]},
};

// --- Forward declarations ---

static void scheduler_thread_exit(void* retval);

// --- Thread lifecycle ---

void scheduler_thread_exit(void* retval)
{
  thread_t* thread = scheduler_thread_current();
  WITH_INTERRUPTS_DISABLED
  {
    WITH_SPINLOCK(&thread->state_lock)
    {
      thread_context_terminated_init(&thread->context, retval);
    }
    WITH_SPINLOCK_END
  }
  WITH_INTERRUPTS_DISABLED_END
  scheduler_state_machine_process_event(thread, THREAD_EVENT_TERMINATE);
}

// --- Idle thread ---

static __attribute__((noreturn)) void* scheduler_thread_idle(__attribute__((unused)) void* const arg)
{
#ifdef DEBUG
  ATOM_ASSERT(interrupts_are_enabled(), "Interrupts must be enabled in idle thread");
#endif
  for (;;)
  {
    wfi();
  }
}

// --- Thread Initialisation ---

static void scheduler_thread_init_common(thread_t* const thread, uint32_t* const stack_base, size_t const stack_size)
{
  thread->stack_base = stack_base;
  thread->stack_size = stack_size;
  thread->state = THREAD_NEW;
  thread->state_lock = SPINLOCK_INITIALIZER;
  thread->scheduler_node = LIST_NODE_INITIALIZER;
  thread->waiters = LIST_INITIALIZER;
  thread->waiters_spinlock = SPINLOCK_INITIALIZER;
}

static void scheduler_thread_init_bootstrap(thread_t* thread, uint32_t* stack_base, size_t const stack_size)
{
  scheduler_thread_init_common(thread, stack_base, stack_size);
  thread->sp = PTR_OFFSET(stack_base, 1, stack_size);
  thread->state = THREAD_RUNNING;
  thread->deadline = scheduler_timestamp_now();
  execution_context[CPUID].current_thread = thread;
}

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
  *(--sp) = (uint32_t)&scheduler_thread_exit; // LR
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

static __attribute__((noreturn)) void scheduler_enter_idle(void)
{
  scheduler_thread_idle(NULL); // Enter idle loop on core 1
}

// --- Scheduler initialization ---

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
  WITH_INTERRUPTS_DISABLED_END
}

void scheduler_init(void)
{
  extern uint32_t const _sstack0;
  extern uint32_t const _estack0;
  if (CPU_IS_CORE_0)
  {
    scheduler_thread_init_bootstrap(&bootstrap_thread, (uint32_t*)&_sstack0, ((uintptr_t)&_estack0 - (uintptr_t)&_sstack0));

    scheduler_thread_init(&deferred_task_thread, deferred_task_thread_stack, DEFERRED_TASK_THREAD_STACK_SIZE, scheduler_deferred_task_worker, NULL);
    deferred_task_context.thread = &deferred_task_thread;
    scheduler_state_machine_process_event(&deferred_task_thread, THREAD_EVENT_START);

    scheduler_thread_init(&idle_thread[CPUID], idle_thread_stack[CPUID], IDLE_THREAD_STACK_SIZE, scheduler_thread_idle, NULL);
    scheduler_state_machine_process_event(&idle_thread[CPUID], THREAD_EVENT_START);
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

// --- Thread selection ---

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
  WITH_SPINLOCK_END
  return next;
}

static inline thread_t* scheduler_pick_thread_on_current_core(void)
{
  return scheduler_pick_next_thread_on_core(CPUID);
}

static inline thread_t* scheduler_steal_thread_on_other_core(void)
{
  return scheduler_pick_next_thread_on_core(CPU_IS_CORE_0 ? 1 : 0);
}

thread_t* scheduler_pick_next_thread(void)
{
  thread_t* next = scheduler_pick_thread_on_current_core();
  if (next == NULL)
  {
    next = scheduler_steal_thread_on_other_core();
  }
  return next != NULL ? next : execution_context[CPUID].idle_thread;
}

// --- Context switching ---

void scheduler_save_current_sp(uint32_t* sp)
{
  execution_context[CPUID].current_thread->sp = sp;
}

uintptr_t scheduler_switch_to(thread_t* next)
{
  thread_t* previous = execution_context[CPUID].current_thread;

  execution_context[CPUID].current_thread = next;
  next->deadline = scheduler_timestamp_add(scheduler_timestamp_now(), scheduler_task_quantum);

  scheduler_state_machine_process_event(previous, THREAD_EVENT_YIELD);
  scheduler_state_machine_process_event(next, THREAD_EVENT_RUN);

  return (uintptr_t)&next->sp;
}
