#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

/**
* @defgroup util Util
* @{
*/

/**
 * @defgroup log Log
 * @ingroup util
 * @brief Lightweight and portable logging framework with customizable formatter and output targets.
 *
 * The logging module provides a simple but extensible logging system suitable
 * for embedded and desktop environments.
 *
 * It separates **formatting** from **output**:
 * - The **formatter** controls how messages look (e.g., prefix, timestamp, etc.).
 * - The **output** determines where messages are written (e.g., `stdout`, file, UART).
 *
 * By default, messages are formatted using ::log_default_formatter
 * and written to ::stdout.
 *
 * Example:
 * @code
 * log_set_min_level(LOG_LEVEL_INFO);
 * log_set_printer(log_default_printer);
 * log_set_output(stdout);
 *
 * log_info("System initialized with version %d", version);
 * @endcode
 * @{
 */

/** Logging severity levels. */
typedef enum
{
  LOG_LEVEL_FATAL = 0, /**< Critical or fatal errors; always shown */
  LOG_LEVEL_ERROR, /**< Recoverable errors */
  LOG_LEVEL_WARN, /**< Recoverable warnings */
  LOG_LEVEL_INFO, /**< Informational messages; default level */
  LOG_LEVEL_DEBUG, /**< Verbose debug details; shown only in debug builds */
} log_level_t;

/**
 * @brief Function type for log formatters.
 *
 * A *formatter* converts a log message and its metadata into
 * human-readable text (e.g., prefixing with log level or timestamp).
 * The formatted output is written to the currently configured output
 * stream (see ::log_set_output).
 *
 * @param output The output where the file will be printed.
 * @param level Log severity level.
 * @param fmt   printf-style format string.
 * @param ...   Arguments corresponding to the format string.
 *
 * @note Custom formatters should be thread-safe and reentrant.
 * @note Formatter may be called from any context (thread or ISR).
 */
typedef void (*log_printer_func_t)(FILE* output, log_level_t level, const char* fmt, va_list args);

/**
 * @brief Configure the output stream.
 *
 * Defines the destination for all log messages. Defaults to ::stdout.
 * This can be redirected to a file or a custom `FILE*` implementation (e.g. UART).
 *
 * @param output The output stream (e.g., stdout, stderr, file handle).
 *
 * @note Not thread-safe with respect to concurrent log calls. Call during initialization.
 * @warning Do not call while other threads are logging; may cause race conditions.
 */
void log_set_output(FILE* output);

/**
 * @brief Set the minimum log level.
 *
 * Messages below this level will be ignored.
 * The default value is INFO
 * @param level Minimum level to output.
 *
 * @note Not thread-safe with respect to concurrent log calls. Call during initialization.
 * @warning Do not call while other threads are logging; may cause race conditions.
 */
void log_set_min_level(log_level_t level);

/**
 * @brief Set the active log printer.
 *
 * Defines how messages are printed.
 * @param printer_func The printer function (e.g. ::log_default_printer).
 *
 * @note Not thread-safe with respect to concurrent log calls. Call during initialization.
 * @warning Do not call while other threads are logging; may cause race conditions.
 */
void log_set_printer(log_printer_func_t printer_func);

/**
 * @brief Prints a formatted log message with severity level.
 *
 * @param level Log severity (error, warn, info, debug).
 * @param fmt   Format string (printf-style).
 * @param ...   Format arguments.
 *
 * @note Thread-safe; safe to call from any context (thread or ISR).
 * @note Filtered by configured minimum log level; messages below the level are ignored.
 */
void log_print(log_level_t level, char const* fmt, ...);

/**
 * @brief Prints an fatal message (level = FATAL).
 * @param fmt Format string.
 * @param ... Format arguments.
 * @note Thread-safe; safe to call from any context (thread or ISR).
 */
#define log_fatal(fmt, ...) log_print(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

/**
 * @brief Prints an error message (level = ERROR).
 * @param fmt Format string.
 * @param ... Format arguments.
 * @note Thread-safe; safe to call from any context (thread or ISR).
 */
#define log_error(fmt, ...) log_print(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

/**
 * @brief Prints a warning message (level = WARN).
 * @param fmt Format string.
 * @param ... Format arguments.
 * @note Thread-safe; safe to call from any context (thread or ISR).
 */
#define log_warn(fmt, ...) log_print(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/**
 * @brief Prints an informational message (level = INFO).
 * @param fmt Format string.
 * @param ... Format arguments.
 * @note Thread-safe; safe to call from any context (thread or ISR).
 */
#define log_info(fmt, ...) log_print(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/**
 * @brief Prints a debug message (level = DEBUG).
 * @param fmt Format string.
 * @param ... Format arguments.
 * @note Thread-safe; safe to call from any context (thread or ISR).
 * @note Only shown if log level is set to LOG_LEVEL_DEBUG or equivalent.
 */
#define log_debug(fmt, ...) log_print(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

/**
 * @brief Default human-readable formatter.
 *
 * Produces output like:
 * @code
 * [INFO] Initialization complete
 * @endcode
 *
 * @note Thread-safe; safe to use as a log printer function.
 */
void log_default_printer(FILE* output, log_level_t level, char const* fmt, va_list args);

/** @} */ /* end of log group */

/** @} */ /* end of util group */

#ifdef __cplusplus
}
#endif
