#include <stdint.h>
#include <string.h>
#include "rp2040/system/cpu.h"

// --- Linker-defined symbols ---
extern uint32_t const _sdata;
extern uint32_t const _edata;
extern uint32_t const _la_data;
extern uint32_t const _sbss;
extern uint32_t const _ebss;
extern uint32_t const _estack0;

// --- External functions ---
void board_init(void);
void __libc_init_array(void);
int main(void);
void __libc_fini_array(void);
__attribute__((noreturn)) void _exit(int status);

static __attribute__((always_inline)) inline void startup_init_stack(void const* stack_top)
{
  __asm__ volatile("msr psp, %0" :: "r"(stack_top) : "memory");
  stack_set_mode(STACK_MODE_PSP);
}

// --- Copy initialized data from flash to RAM ---
static void startup_init_data(void)
{
  size_t const size = (uintptr_t)&_edata - (uintptr_t)&_sdata;
  memcpy((void*)&_sdata, &_la_data, size);
}

// --- Zero-initialize the .bss section ---
static void startup_init_bss(void)
{
  size_t const size = (uintptr_t)&_ebss - (uintptr_t)&_sbss;
  memset((void*)&_sbss, 0, size);
}

// --- Reset handler (main program entry point) ---
__attribute__((noreturn, used)) void irq_handler_reset(void)
{
  startup_init_stack(&_estack0);
  startup_init_data();
  startup_init_bss();
  __libc_init_array();
  board_init();
  main();
  _exit(1);
}
