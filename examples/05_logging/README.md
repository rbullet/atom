# Logging Example

This example demonstrates how to use the ATOM logging system to output diagnostic messages with different severity levels.

The application simulates a small application lifecycle and generates logs for normal information, debugging data, warnings, and errors.

## Demonstrates

* Initializing the logger
* Configuring the minimum log level
* Using different log severity levels
* Formatting log messages with runtime values

## Source

```c
#include <atom.h>

int main(void)
{
  log_set_min_level(LOG_LEVEL_DEBUG);

  log_info("Starting the application");

  char const* player_name = "Alice";
  uint32_t health = 100;

  log_debug("Player %s started with %u HP", player_name, health);

  health = 20;
  log_warn("Player health is low!");

  //...

  log_error("Failed to save your progress");

  while (1)
  {
  }
}
```

## Expected output

The logger automatically includes the log level and source location:

```text
[INFO] main.c:7 Starting the application
[DEBUG] main.c:12 Player Alice started with 100 HP
[WARN] main.c:15 Player health is low!
[ERROR] main.c:17 Failed to save your progress
```

The exact line numbers may vary depending on the source file.

## Log levels

ATOM provides multiple log levels ordered by severity:

| Level             | Purpose                                                        |
| ----------------- | -------------------------------------------------------------- |
| `LOG_LEVEL_FATAL` | Critical failures that prevent the application from continuing |
| `LOG_LEVEL_ERROR` | Recoverable errors requiring attention                         |
| `LOG_LEVEL_WARN`  | Unexpected situations that do not necessarily indicate failure |
| `LOG_LEVEL_INFO`  | General application information                                |
| `LOG_LEVEL_DEBUG` | Detailed diagnostic information useful during development      |

The minimum log level controls which messages are displayed.

For example:

```c
log_set_min_level(LOG_LEVEL_INFO);
```

will display:

* `LOG_LEVEL_FATAL`
* `LOG_LEVEL_ERROR`
* `LOG_LEVEL_WARN`
* `LOG_LEVEL_INFO`

but will hide:

* `LOG_LEVEL_DEBUG`

To enable all log messages during development:

```c
log_set_min_level(LOG_LEVEL_DEBUG);
```

## Purpose

Logging provides a structured way to observe what is happening inside an application.

Compared to using `printf()` directly, the ATOM logger provides additional context such as:

* Message severity
* Source file
* Line number
* Filtering based on importance

This makes debugging embedded applications easier, especially as applications grow in complexity.

## Notes

Logging levels allow developers to control the amount of information produced by the application.

A typical development workflow is:

* Use `LOG_LEVEL_DEBUG` while developing and troubleshooting.
* Use `LOG_LEVEL_INFO` or higher in normal operation to reduce output.
* Disable logging entirely when required using `LOG_DISABLED`.

The logging system is designed to provide useful diagnostics without requiring developers to remove logging statements from their code.
