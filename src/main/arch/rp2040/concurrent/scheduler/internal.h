#pragma once

#include "rp2040/atom.h"
#include "rp2040/concurrent/scheduler.h"

// --- Private Peripheral Bus (PPB) ---
#define PPB_BASE                            0xE0000000

// --- Interrupt Control and State Register (ICSR) ---
#define PPB_ICSR_OFFSET                     0xED04
#define ICSR                                REG(PPB_BASE, PPB_ICSR_OFFSET)
#define ICSR_PENDSVSET                      (1u << 28)

// --- SysTick Registers ---
#define PPB_SYST_CSR_OFFSET                 0xE010
#define PPB_SYST_RVR_OFFSET                 0xE014
#define PPB_SYST_CVR_OFFSET                 0xE018
#define PPB_SYST_CALIB_OFFSET               0xE01C

#define SYST_CSR                            REG(PPB_BASE, PPB_SYST_CSR_OFFSET)
#define SYST_RVR                            REG(PPB_BASE, PPB_SYST_RVR_OFFSET)
#define SYST_CVR                            REG(PPB_BASE, PPB_SYST_CVR_OFFSET)
#define SYST_CALIB                          REG(PPB_BASE, PPB_SYST_CALIB_OFFSET)

#define PPB_SYST_CSR_ENABLE_MASK           0x1
#define PPB_SYST_CSR_ENABLE_OFFSET         0

#define PPB_SYST_RVR_RELOAD_MASK           0x00FFFFFF
#define PPB_SYST_RVR_RELOAD_OFFSET         0

#define PPB_SYST_CVR_CURRENT_MASK          0x00FFFFFF
#define PPB_SYST_CVR_CURRENT_OFFSET        0

#define SYST_CSR_ENABLE                    (1u << 0)
#define SYST_CSR_TICKINT                   (1u << 1)
#define SYST_CSR_CLKSOURCE                 (1u << 2)

// --- NVIC Priority Registers ---
#define PPB_NVIC_IPR3_OFFSET               0xE40C
#define NVIC_IPR3                          REG(PPB_BASE, PPB_NVIC_IPR3_OFFSET)

#define PPB_NVIC_IPR3_IP_14_MASK           0x00C00000
#define PPB_NVIC_IPR3_IP_14_OFFSET         22

#define PPB_NVIC_IPR3_IP_15_MASK           0xC0000000
#define PPB_NVIC_IPR3_IP_15_OFFSET         30

// --- Scheduler Configuration ---
#define XPSR_THREAD                        0x01000000   // Default xPSR for new threads
#define IDLE_THREAD_STACK_SIZE             1024         // Idle thread stack size (bytes)
#define DEFERRED_TASK_THREAD_STACK_SIZE    1024         // Deferred task thread stack size (bytes)

#define THREAD_STACK_CANARY 0xDEADBEEF

#define DEFERRED_TASK_EXPIRED 1U << 0

extern volatile uint64_t sys_tick;

typedef struct
{
  spinlock_t spinlock;
  sorted_list_t tasks_queue;
  event_flags_t event_flags;
  thread_t* thread;
} deferred_task_context_t;

extern thread_t deferred_task_thread;

extern deferred_task_context_t deferred_task_context;

void* scheduler_deferred_task_callback(void* arg);

typedef struct
{
  spinlock_t spinlock;
  list_t ready_queue;
  thread_t* current_thread;
  thread_t* idle_thread;
} execution_context_t;

extern execution_context_t execution_context[CPU_COUNT];

typedef enum
{
  THREAD_EVENT_NONE,
  THREAD_EVENT_START,
  THREAD_EVENT_RUN,
  THREAD_EVENT_YIELD,
  THREAD_EVENT_BLOCK,
  THREAD_EVENT_SLEEP,
  THREAD_EVENT_WAKEUP,
  THREAD_EVENT_TERMINATE,

  THREAD_EVENT_COUNT
} thread_event_t;

bool thread_process_event(thread_t *thread, thread_event_t event);

void scheduler_request_context_switch(void);

// -- ISRs
void scheduler_sys_tick_handler(void);

void scheduler_pendsv_handler(void);
