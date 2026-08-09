# ATOM

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Language: C](https://img.shields.io/badge/Language-C-blue.svg)](...)
[![Platform: RP2040](https://img.shields.io/badge/Platform-RP2040-green.svg)](https://www.raspberrypi.com/products/rp2040/)
[![Architecture: ARM Cortex--M0+](https://img.shields.io/badge/Architecture-ARM%20Cortex--M0%2B-orange.svg)](https://developer.arm.com/Processors/Cortex-M0-Plus)
[![Build System: CMake](https://img.shields.io/badge/Build-CMake-red.svg)](...)
![Build Status](https://github.com/rbullet/atom/actions/workflows/cmake-single-platform.yml/badge.svg)

**ATOM** is a lightweight educational bare-metal SMP framework for the Raspberry Pi Pico (RP2040).

It provides a small RTOS-like execution environment built from scratch, including
preemptive scheduling, threads, synchronization primitives, peripheral drivers, and
embedded utilities without relying on an external RTOS.

The project is designed to explore how operating system concepts are implemented
on resource-constrained microcontrollers.

The project focuses on:

- Simple, readable APIs
- True dual-core SMP scheduling
- Explicit resource ownership
- No kernel heap allocation
- Application-owned stacks and resources
- Predictable execution

---

# Project History & Motivation

ATOM was created to understand how operating system concepts work by implementing
them directly on a microcontroller.

It started as a small experimental project on an Arduino board before evolving
into an RP2040 bare-metal framework exploring scheduling, context switching,
synchronization, and multicore execution.

The name **ATOM** comes from the editor used during the early versions of the
project: the [Atom editor](https://atom.io/). Although the editor is no longer
maintained, the name remained as a tribute to the tool that accompanied the
project's beginnings.

ATOM is not intended to compete with production RTOSes. Its goal is to provide
a small and understandable implementation where the mechanisms behind an
operating system can be explored.

---

# Features

## Highlights

- True dual-core SMP scheduler
- Secondary processor started explicitly on demand
- No kernel dynamic allocation
- `main()` as the first scheduled thread
- Configurable CPU frequency
- Pluggable time source
- C runtime support with UART0 console output

## Scheduler

- Preemptive SMP scheduling
- Per-core ready queues with cross-core work stealing
- PendSV context switching
- 1 ms scheduling tick
- Explicit cooperative yielding (`thread_yield()`)
- Blocking synchronization primitives
- Secondary scheduler started explicitly with
  `scheduler_start_secondary()`

By default, ATOM starts scheduling on the primary processor only.
Applications that want dual-core scheduling must explicitly start the
secondary processor:

``` c
scheduler_start_secondary();
```

This makes single-core operation the default while allowing applications
to opt into SMP execution when required.

## Concurrency

- Threads
- Thread wait / notify
- Thread join with return values
- Recursive mutexes
- Counting semaphores
- Condition variables
- Event flags
- Hardware spinlocks
- Deferred tasks
- Scoped interrupt masking
- Scoped resource guards (mutex, semaphore, spinlock, interrupts)

## Drivers

- GPIO
- UART
- Clock initialization
- Logging

## Utilities

- Time abstractions
- Linked lists
- Sorted lists
- Assertions
- Helper macros
- Configurable logging framework

---

# Example

```c
#include <stdbool.h>

#include <atom.h>

#define LED_PIN 25

static thread_t worker;
static uint32_t worker_stack[256];

static thread_t led_blink_thread;
static uint32_t led_blink_thread_stack[256];

static void* worker_thread(void* arg)
{
  (void)arg;

  while (true)
  {
    log_info("Hello from worker");
    thread_sleep(duration_of(1, SECONDS));
  }

  return NULL;
}

static void* blink_led_thread(void* arg)
{
  uint32_t pin = (uint32_t)(uintptr_t)arg;
  while (true)
  {
    gpio_toggle(pin);
    thread_sleep(duration_of(1, SECONDS));
  }

  return NULL;
}

int main(void)
{
  // main() already executes as a schedulable thread.
  
  gpio_config(LED_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);

  thread_init(&led_blink_thread, led_blink_thread_stack, sizeof(led_blink_thread_stack), blink_led_thread, (void*)(uintptr_t)LED_PIN);
  thread_start(&led_blink_thread);

  thread_init(&worker, worker_stack, sizeof(worker_stack), worker_thread, NULL);
  thread_start(&worker);

  while (true)
  {
    log_info("Hello from main");
    thread_sleep(duration_of(5, SECONDS));
  }
}

```
> [!NOTE] `main()` is the application's initial thread. The scheduler
> is already running before `main()` is entered, so there is no
> `scheduler_start()` function.
>
> ATOM starts on the primary processor by default. If the application
> wants to use both RP2040 processors, it must explicitly call
> `scheduler_start_secondary()`.
>
> Threads created with `thread_start()` are scheduled automatically
> alongside the main thread.

This snippet mirrors `examples/07_thread`. The `examples/` directory
contains a progression of self-contained, standalone sample
applications, each demonstrating one feature in isolation:

| Example | Demonstrates                                      |
|---|---------------------------------------------------|
| `00_developer_playground` | Scratch template                                  |
| `01_hello_world` | C runtime and UART output                         |
| `02_blink` | GPIO and sleeping threads                         |
| `03_stdin_stdout` | Console input/output                              |
| `04_assert` | Assertions                                        |
| `05_logging` | Logging framework                                 |
| `06_timer` | Periodic deferred tasks                           |
| `07_thread` | Multiple threads                                  |
| `08_thread_result` | Thread join and return values                     |
| `09_mutex` | Mutex synchronization                             |
| `10_semaphore` | Producer/consumer synchronization                 |
| `11_condition_variable` | Wait/broadcast synchronization                    |
| `12_event_flags` | Persistent event state synchronization            |
| `13_work_stealing` | Dual-core scheduling and cross-core work stealing |

---

# Scoped Resource Guards

Mutexes, semaphores and interrupt masking all support a scoped, RAII-style
block form that acquires the resource on entry and releases it automatically
on exit, including early `return`:

```c
WITH_MUTEX(&resource_lock)
{
  update_shared_state();
}

WITH_SEMAPHORE(&pool_semaphore)
{
  use_pooled_resource();
}

WITH_INTERRUPTS_DISABLED
{
  critical_update();
}
```

This avoids common bugs where an early `return` or exception path forgets to
release a lock.

`WITH_SPINLOCK(lock)` follows the same pattern:

```c
spinlock_t* lock = spinlock_pool_reserve(SPINLOCK_POOLED);

WITH_SPINLOCK(lock)
{
    update_shared_hardware_state();
}
```

The public spinlock API does not expose hardware spinlock registers directly.
Hardware spinlocks are managed through the ATOM spinlock pool allocator.

Applications can request:
- exclusive spinlocks for dedicated ownership,
- pooled spinlocks for dynamically created synchronization objects.

This keeps hardware resource ownership centralized and prevents conflicts between
application code and ATOM internals.

---

# C Runtime Support

ATOM integrates with newlib, providing a standard C runtime environment on bare metal.

Applications can use standard C APIs such as:

- `printf()`
- `puts()`
- `getchar()`
- `fgets()`
- `malloc()` / `free()`

UART0 is configured as the default console, connecting stdin/stdout to the board automatically.

For example, this standard C program can run on both a desktop environment and ATOM:

```c
#include <stdio.h>

#define BUFFER_SIZE 128

int main(void)
{
    char buffer[BUFFER_SIZE];

    printf("Console echo ready.\r\n");

    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        printf("echo: %s", buffer);
    }

    return 0;
}
```

ATOM provides the required newlib syscall integration, including console I/O
and thread-safe heap support.

Heap usage is optional and remains application-owned. The scheduler and
synchronization primitives do not depend on dynamic allocation.

---

# Building

## Requirements

- ARM GNU Toolchain
- CMake 3.24+
- Ninja (or Make)
- OpenOCD (optional)

## Build

ATOM builds as a static library (`libatom.a`). Application executables (including
the bundled examples) link against it.

```bash
git clone https://github.com/rbullet/atom
cd atom

mkdir cmake-build
cd cmake-build

cmake \
    -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release ..

ninja
```

This produces the static library:

```
build/libatom.a
```

along with any example executables (e.g. `02_blink`, `09_mutex`), which are
placed under their respective `examples/<name>/build/` directories.

To build your own application, link against `libatom.a` and add `include/` to
your include path, in the same way the bundled examples do:

```cmake
target_link_libraries(my_app PRIVATE
        "-Wl,--whole-archive"
        atom
        "-Wl,--no-whole-archive"
)
```

Create a UF2 image from an example (or your own executable):

```bash
../tools/elf2uf2 examples/02_blink/build/02_blink -o 02_blink.uf2
```

Copy the UF2 onto the Pico while it is in BOOTSEL mode.

---

# Architecture

ATOM implements a lightweight symmetric multiprocessing scheduler.

Both Cortex-M0+ cores execute application threads concurrently.

Each core owns its own ready queue. When a core's queue runs empty, it steals
a runnable thread from the other core's queue before falling back to its idle
thread, which keeps both cores fed without a single shared/contended queue.

```
        Core 0                              Core 1
   ┌──────────────────────────┐                   ┌──────────────────────────┐
   │  Ready Queue             │◄─── work steal ──►│  Ready Queue             │
   │ (execution_              │                   │ (execution_              │
   │  context[0].splinlock)   │                   │  context[1].splinlock)   │
   └───────────┬──────────────┘                   └─────────────┬────────────┘
               │                                                │
               ▼                                                ▼
           +---------+                                     +---------+
           | Core 0  |                                     | Core 1  |
           +---------+                                     +---------+
           | SysTick |                                     | SysTick |
           | PendSV  |                                     | PendSV  |
           +---------+                                     +---------+
```

Features:

- Per-core ready queues, protected by per-core hardware spinlocks
- Cross-core work stealing: an idle core pulls a runnable thread from the
  other core's queue instead of sitting idle while work is available
- Independent context switching per core
- Core 0 maintains the global scheduler tick / time base
- Secondary-core startup is explicit and application-controlled

---

# Project Layout

```
atom/

├── include/
│   ├── concurrent/
│   ├── io/
│   └── util/
│
├── src/
│   └── main/
│       └── arch/rp2040/
│
├── examples/
│   ├── 00_developer_playground/
│   ├── 01_hello_world/
│   ├── 02_blink/
│   ├── 03_stdin_stdout/
│   ├── 04_assert/
│   ├── 05_logging/
│   ├── 06_timer/
│   ├── 07_thread/
│   ├── 08_thread_result/
│   ├── 09_mutex/
│   ├── 10_semaphore/
│   ├── 11_condition_variable/
│   ├── 12_event_flags/
│   └── 13_work_stealing/
│
├── build/
│   └── libatom.a
│
├── doc/
├── tools/
└── CMakeLists.txt
```

ATOM itself builds as a static library (`libatom.a`); the `examples/` directory
contains standalone executables that link against it, and each is
self-contained enough to be copied out and used as the starting point for a new
project.

---

# Concurrency Primitives

| Primitive | Description |
|------------|-------------|
| Threads | Lightweight execution contexts, with wait/notify and join support |
| Mutex | Recursive mutual exclusion |
| Semaphore | Counting synchronization |
| Condition Variable | Wait/signal synchronization |
| Event Flags | Persistent state-based synchronization using bit flags |
| Spinlock | Hardware-backed cross-core locking. Hardware resources are managed through the spinlock pool allocator. |
| Deferred Task | Delayed or periodic callbacks |
| Interrupt Control | Scoped interrupt masking |
| Scoped Guards | `WITH_MUTEX` / `WITH_SEMAPHORE` / `WITH_INTERRUPTS_DISABLED` blocks |

Non-blocking variants are available for contended resources
(`mutex_try_lock()`, `semaphore_try_acquire()`).

---

# Peripheral Drivers

## GPIO

- Input/output
- Pull-up/down
- Read/write
- Toggle
- Safe to call from any execution context, including interrupts
- `gpio_init()` is invoked automatically during board initialization;
  application code does not need to (and should not) call it again, since
  doing so would reset pin muxing already set up for other peripherals (e.g.
  UART TX/RX)

## UART

- UART0
- UART1
- Configurable baud rate
- Blocking API only (no interrupt-driven/async I/O)

## Logging

- Severity levels: `FATAL`, `ERROR`, `WARN`, `INFO`, `DEBUG`
- Pluggable message formatter (`log_set_printer()`)
- Redirectable output stream, defaults to `stdout` (`log_set_output()`)
- Runtime-configurable minimum level (`log_set_min_level()`)
- Safe to call from thread or interrupt context

```c
log_set_min_level(LOG_LEVEL_INFO);
log_info("System initialized with version %d", version);
```

---

# Documentation

The project documentation is generated with **Doxygen** using the excellent
[Doxygen Awesome](https://jothepro.github.io/doxygen-awesome-css/) theme by
Jan T. Schaub.

Generate the complete API reference with:

```bash
ninja doc
```

After the build completes, a convenience symlink is created at:

```text
doc/index.html
```

which points to the generated documentation in:

```text
${CMAKE_BINARY_DIR}/doc/html/index.html
```

This allows the documentation to be opened directly from the repository without
having to navigate into the build directory.

---

# Debugging

Start OpenOCD:

```bash
openocd -f openocd_pico.cfg
```

Connect using GDB, targeting one of the built example executables (`atom`
itself is a static library and cannot be loaded/debugged directly):

```gdb
arm-none-eabi-gdb examples/02_blink/build/02_blink

(gdb) target remote :3333
(gdb) load
(gdb) break main
(gdb) continue
```

---

# Configuration and Extensibility

## Independent Time Sources

ATOM separates kernel scheduling time from the application timestamp API.

The scheduler uses its own internal time base for scheduling operations.

The public timestamp API can be replaced by applications without modifying
application code or affecting scheduler behavior.

For example:

```c
thread_sleep(duration_of(5, SECONDS));
```

Whether the application timestamp implementation uses the default timer, a hardware timer, an RTC, or another source, existing application code remains unchanged.

## CPU Clock

The CPU frequency is configured at build time.

ATOM does not assume a fixed clock frequency. The system clock can be
overclocked or underclocked by changing the build configuration.

Example:

```cmake
set(CPU_FREQUENCY 125000000)
```

## Default UART Baud Rate

The default console baud rate is also a build-time setting, alongside
`CPU_FREQUENCY`:

```cmake
set(UART_BAUD_RATE 115200)
```

Both values are generated into `atom_config.h` at configure time.

---

# Limitations

Current limitations include:

- FIFO scheduler (no priorities)
- Shared vector table
- Core 0 owns the global time base
- No memory protection
- No userspace/kernel separation
- Application-managed thread stacks
- Blocking-only UART driver (no interrupt-driven/async I/O)

---

# Contributing

Contributions, ideas, and feedback are always welcome.

If you've found a bug, have a feature request, or would like to propose an improvement:

Open an Issue to report bugs or request new features.
Start a Discussion to ask questions or share ideas.
Submit a Pull Request if you'd like to contribute code or documentation.

Whether it's a bug report, a suggestion, a documentation improvement, or a code contribution, every contribution is appreciated.

---

# License

See the LICENSE file.

---

# Acknowledgments

ATOM draws inspiration from established embedded kernels such as FreeRTOS and Zephyr while remaining an independent implementation.

Special thanks to the Doxygen Awesome project for providing the documentation
theme used by ATOM.

- https://jothepro.github.io/doxygen-awesome-css/

---

# References

- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [ARM Cortex-M0+ Generic User Guide](https://support.arm.com/documentation/dui0662/b/)
- [Raspberry Pi Pico Documentation](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html)