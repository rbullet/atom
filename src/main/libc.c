#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>

// --- Thread-safe malloc hooks ---
// Weak implementations; can be overridden if desired.
// Protects heap allocation (malloc, free, etc.) across threads.
__attribute__((used, weak)) void __malloc_lock(struct _reent const *r)
{
  (void)r;
}

__attribute__((used, weak)) void __malloc_unlock(struct _reent const *r)
{
  (void)r;
}

// --- Increase program data space (heap) ---
// Weak implementation of `_sbrk`. Target-specific code may override this.
// Returns previous heap end on success, or (void*)-1 if out of memory.
__attribute__((used, weak)) void* _sbrk(ptrdiff_t const incr)
{
  (void)incr;
  errno = ENOMEM;
  return (void*)-1;
}

// --- Close a file descriptor ---
// Weak stub for `_close`, typically overridden in target-specific code.
__attribute__((used, weak)) int _close(int const file)
{
  (void)file;
  return -1;
}

// --- File status ---
// Weak stub for `_fstat`. Marks file as character device (S_IFCHR).
__attribute__((used, weak)) int _fstat(int const file, struct stat* st)
{
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

// --- Check if file descriptor is a terminal ---
// Weak stub for `_isatty`.
__attribute__((used, weak)) int _isatty(int const file)
{
  (void)file;
  return 1;
}

// --- Reposition read/write file offset ---
// Weak stub for `_lseek`.
__attribute__((used, weak)) off_t _lseek(int const file, off_t const offset, int const whence)
{
  (void)file;
  (void)offset;
  (void)whence;
  return 0;
}

// --- Read from a file descriptor ---
// Weak stub for `_read`. Returns 0 bytes read.
__attribute__((used, weak)) size_t _read(int const file, uint8_t* ptr, size_t const len)
{
  (void)file;
  (void)ptr;
  (void)len;
  return 0;
}

// --- Write to a file descriptor ---
// Weak stub for `_write`. Pretends to write all bytes successfully.
__attribute__((used, weak)) size_t _write(int const file, uint8_t const* ptr, size_t const len)
{
  (void)file;
  (void)ptr;
  return len;
}

// --- Terminate the program ---
// Weak stub for `_exit`. Enters infinite wait-for-event loop.
__attribute__((used, weak, noreturn)) void _exit(int status)
{
  (void)status;
  while (1)
  {
  }
}

// --- Send signal to process ---
// Weak stub for `_kill`.
__attribute__((used, weak)) int _kill(int const pid, int const sig)
{
  (void)pid;
  (void)sig;
  return -1;
}

// --- Get process ID ---
// Weak stub for `_getpid`.
__attribute__((used, weak)) int _getpid(void)
{
  return 1;
}

// --- Get current time of day ---
// Weak stub for `_gettimeofday`.
__attribute__((used, weak)) int _gettimeofday(struct timeval const* tv, void const* tz)
{
  (void)tv;
  (void)tz;
  return 0;
}