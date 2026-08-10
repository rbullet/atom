#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>

// --- newlib syscall stubs ---

void __attribute__((used, weak)) __malloc_lock(struct _reent const* r)
{
  (void)r;
}

void __attribute__((used, weak)) __malloc_unlock(struct _reent const* r)
{
  (void)r;
}

void* __attribute__((used, weak)) _sbrk(ptrdiff_t const incr)
{
  (void)incr;
  errno = ENOMEM;
  return (void*)-1;
}

int __attribute__((used, weak)) _close(int const file)
{
  (void)file;
  return -1;
}

int __attribute__((used, weak)) _fstat(int const file, struct stat* st)
{
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

int __attribute__((used, weak)) _isatty(int const file)
{
  (void)file;
  return 1;
}

off_t __attribute__((used, weak)) _lseek(int const file, off_t const offset, int const whence)
{
  (void)file;
  (void)offset;
  (void)whence;
  return 0;
}

size_t __attribute__((used, weak)) _read(int const file, uint8_t* const ptr, size_t const len)
{
  (void)file;
  (void)ptr;
  (void)len;
  return 0;
}

size_t __attribute__((used, weak)) _write(int const file, uint8_t const* ptr, size_t const len)
{
  (void)file;
  (void)ptr;
  return len;
}

void __attribute__((used, weak, noreturn)) _exit(int const status)
{
  (void)status;
  while (1)
  {
  }
}

int __attribute__((used, weak)) _kill(int const pid, int const sig)
{
  (void)pid;
  (void)sig;
  return -1;
}

int __attribute__((used, weak)) _getpid(void)
{
  return 1;
}

int __attribute__((used, weak)) _gettimeofday(struct timeval const* tv, void const* tz)
{
  (void)tv;
  (void)tz;
  return 0;
}
