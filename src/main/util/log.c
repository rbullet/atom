#include "util/log.h"
#include <stdio.h>
#include <time.h>

static FILE* log_stream = NULL;

static char const * const LEVEL_NAMES[] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG"};

static log_printer_func_t log_printer_func = log_default_printer;

static log_level_t log_min_level = LOG_LEVEL_INFO;

static inline char const* log_filename(char const* path);

void log_set_output(FILE* output)
{
  log_stream = output;
}

void log_set_printer(log_printer_func_t const printer_func)
{
  log_printer_func = printer_func;
}

void log_set_min_level(log_level_t const level)
{
  log_min_level = level;
}

void log_print(log_level_t level, char const* file, char const* function, int line, char const* fmt, ...)
{
  if (level > log_min_level)
  {
    return;
  }
  FILE* output = log_stream ? log_stream : stdout;
  va_list args;
  va_start(args, fmt);
  log_printer_func(output, level, file, function, line, fmt, args);
  va_end(args);
}

void log_default_printer(FILE* output, log_level_t level, char const* file, char const* function, int line, char const* fmt, va_list args)
{
  fprintf(output, "[%s][%s:%d] %s: ", LEVEL_NAMES[level], log_filename(file), line, function);
  vfprintf(output, fmt, args);
  fprintf(output, "\r\n");
}

static inline char const* log_filename(char const* path)
{
  char const* file = path;

  for (char const* p = path; *p != '\0'; ++p)
  {
    if (*p == '/' || *p == '\\')
    {
      file = p + 1;
    }
  }

  return file;
}
