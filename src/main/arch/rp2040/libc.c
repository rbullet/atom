#include <stddef.h>
#include <stdint.h>
#include <sys/errno.h>
#include <stdio.h>

#include <atom.h>
#include "rp2040/system/cpu.h"

static spinlock_t global_spinlock = SPINLOCK_INITIALIZER;

__attribute__((used, weak)) ssize_t atom_console_read(void *buf, size_t size);
__attribute__((used, weak)) ssize_t atom_console_write(const void *buf, size_t size);

// --- External symbols for heap boundaries (defined in linker script) ---
extern volatile uint32_t _sheap;
extern uint32_t _eheap;

// --- Pointer to track current end of heap ---
static uint8_t* heap_end = NULL;

static uint32_t interrupt_state[CPU_COUNT];

__attribute__((used)) void __malloc_lock(__attribute__((unused)) struct _reent* r)
{
  interrupt_state[CPUID] = interrupts_disable();
  spinlock_lock(&global_spinlock);
}

__attribute__((used)) void __malloc_unlock(__attribute__((unused)) struct _reent* r)
{
  spinlock_unlock(&global_spinlock);
  interrupts_restore(interrupt_state[CPUID]);
}

__attribute__((used)) void* _sbrk(ptrdiff_t const incr)
{
  if (heap_end == NULL)
  {
    heap_end = (uint8_t*)&_sheap;
  }
  uint8_t* prev_heap_end = heap_end;
  if ((heap_end + incr) > (uint8_t*)&_eheap)
  {
    errno = ENOMEM;
    return (void*)-1;
  }
  heap_end += incr;
  return prev_heap_end;
}

__attribute__((used)) ssize_t _read(__attribute__((unused)) int const file, uint8_t* ptr, size_t const len)
{
  ssize_t const count = (ssize_t)len;
  for (ssize_t i = 0; i < count; i++)
  {
    ssize_t const ret = atom_console_read(&ptr[i], 1);

    if (ret < 0)
    {
      return -1;
    }

    if (ret == 0)
    {
      return i;
    }

    if (ptr[i] == '\n')
    {
      return i + 1;
    }
  }
  return count;
}

__attribute__((used)) ssize_t _write(__attribute__((unused)) int const file, uint8_t const* ptr, size_t const len)
{
  return atom_console_write(ptr, len);
}

__attribute__((used, weak)) ssize_t atom_console_read(void *buf, size_t const size)
{
  return (ssize_t)uart_read(uart0, buf, size);
}

__attribute__((used, weak)) ssize_t atom_console_write(void const *buf, size_t size)
{
  return (ssize_t)uart_write(uart0, buf, size);
}

#undef MALLOC_SPINLOCK
