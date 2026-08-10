#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>

// --- newlib syscall stubs ---

__attribute__((used, weak)) void __malloc_lock(struct _reent const* r)
{
  (void)r;
}

__attribute__((used, weak)) void __malloc_unlock(struct _reent const* r)
{
  (void)r;
}

__attribute__((used, weak)) void* _sbrk(ptrdiff_t const incr)
{
  (void)incr;
  errno = ENOMEM;
  return (void*)-1;
}

__attribute__((used, weak)) int _close(int const file)
{
  (void)file;
  return -1;
}

__attribute__((used, weak)) int _fstat(int const file, struct stat* st)
{
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

__attribute__((used, weak)) int _isatty(int const file)
{
  (void)file;
  return 1;
}

__attribute__((used, weak)) off_t _lseek(int const file, off_t const offset, int const whence)
{
  (void)file;
  (void)offset;
  (void)whence;
  return 0;
}

__attribute__((used, weak)) size_t _read(int const file, uint8_t* ptr, size_t const len)
{
  (void)file;
  (void)ptr;
  (void)len;
  return 0;
}

__attribute__((used, weak)) size_t _write(int const file, uint8_t const* ptr, size_t const len)
{
  (void)file;
  (void)ptr;
  return len;
}

__attribute__((used, weak, noreturn)) void _exit(int status)
{
  (void)status;
  while (1)
  {
  }
}

__attribute__((used, weak)) int _kill(int const pid, int const sig)
{
  (void)pid;
  (void)sig;
  return -1;
}

__attribute__((used, weak)) int _getpid(void)
{
  return 1;
}

__attribute__((used, weak)) int _gettimeofday(struct timeval const* tv, void const* tz)
{
  (void)tv;
  (void)tz;
  return 0;
}
