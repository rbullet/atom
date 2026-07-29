#include <stddef.h>
#include <stdint.h>
#include <sys/errno.h>
#include "rp2040/libc.h"

#include <stdio.h>

#include "concurrent/interrupts.h"
#include "rp2040/config.h"
#include "rp2040/system/cpu.h"
#include "rp2040/concurrent/spinlock.h"

// --- External symbols for heap boundaries (defined in linker script) ---
extern volatile uint32_t _sheap;
extern uint32_t _eheap;

// --- Pointer to track current end of heap ---
static uint8_t* heap_end = NULL;

static volatile libc_read_func_t read_callback = NULL;

static volatile libc_write_func_t write_callback = NULL;

static uint32_t interrupt_state[CPU_COUNT];

libc_read_func_t libc_get_read_callback(void)
{
  return read_callback;
}

void libc_set_read_callback(libc_read_func_t read_func)
{
  read_callback = read_func;
}

libc_write_func_t libc_get_write_callback(void)
{
  return write_callback;
}

void libc_set_write_callback(libc_write_func_t write_func)
{
  write_callback = write_func;
}

__attribute__((used)) void __malloc_lock(__attribute__((unused)) struct _reent* r)
{
  interrupt_state[CPUID] = interrupts_disable();
  spinlock_lock(malloc_spinlock);
}

__attribute__((used)) void __malloc_unlock(__attribute__((unused)) struct _reent* r)
{
  spinlock_unlock(malloc_spinlock);
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
  libc_read_func_t const callback = read_callback;
  if (callback == NULL)
  {
    return -1;
  }
  ssize_t const count = (ssize_t)len;
  for (ssize_t i = 0; i < count; i++)
  {
    ssize_t const ret = callback(&ptr[i], 1);

    if (ret < 0)
    {
      return -1;
    }

    if (ret == 0)
    {
      return i;
    }

    if (ptr[i] == '\r' || ptr[i] == '\n')
    {
      return i + 1;
    }
  }
  return (ssize_t)len;
}

__attribute__((used)) ssize_t _write(__attribute__((unused)) int const file, uint8_t const* ptr, size_t const len)
{
  libc_write_func_t const callback = write_callback;
  if (callback == NULL)
  {
    return -1;
  }
  return callback(ptr, len);
}
