# ATOM

**ATOM** is a lightweight educational bare-metal SMP framework for the Raspberry Pi Pico (RP2040).

It provides a small RTOS-like execution environment built from scratch, including
preemptive scheduling, threads, synchronization primitives, peripheral drivers, and
embedded utilities without relying on an external RTOS.

The project is designed to explore how operating system concepts are implemented
on resource-constrained microcontrollers.

The project focuses on:

- Small and readable implementation
- Explicit resource ownership
- No kernel dynamic allocation
- True dual-core scheduling
- Simple APIs inspired by higher-level environments

---

# Project History & Motivation

The original goal was not to create a production RTOS, but to understand the mechanisms
behind one by implementing them directly on a microcontroller.

ATOM started as a small experimental project on an Arduino board, long before
it became an RP2040 bare-metal framework.

The name **ATOM** comes from the editor I was using at the time: the
[Atom editor](https://atom.io/).

Although the editor is no longer maintained, the name remained as a small tribute
to the tool that accompanied the first versions of the project.

Over time, the project evolved from a simple embedded experiment into a complete
exploration of operating system concepts: scheduling, context switching,
synchronization, and multicore execution on microcontrollers.

---

# Features

## Highlights

- True dual-core SMP scheduler
- No kernel dynamic allocation
- `main()` is the first scheduled thread
- Configurable CPU frequency
- Pluggable timestamp implementation
- Small, readable implementation
- Integrated C runtime support with `printf()` output through UART0 by default

## Design Goals

- Simple APIs
- Small and readable implementation
- No hidden dynamic allocation
- Explicit ownership of resources
- Configurable system clock
- Independent application time source
- Predictable execution

## Scheduler

- Preemptive SMP scheduling
- Shared ready queue
- Thread migration
- PendSV context switching
- 1 ms scheduling tick
- Cooperative yielding
- Blocking synchronization primitives

## Concurrency

- Threads
- Recursive mutexes
- Counting semaphores
- Condition variables
- Hardware spinlocks
- Deferred tasks
- Scoped interrupt masking

## Drivers

- GPIO
- UART
- Clock initialization
- Logging

## Utilities

- Time abstractions
- Linked lists
- Assertions
- Helper macros

---

# Example

```c
#include <stdbool.h>
#include "atom.h"

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
> [!NOTE]
> `main()` is the application's initial thread. The scheduler is already running before
> `main()` is entered, so there is no `scheduler_start()` function. Threads created with
> `thread_start()` are scheduled automatically alongside the main thread.

---

# C Runtime Support

ATOM integrates with the C standard library through newlib, providing a standard
C runtime environment on bare metal.

Applications can use standard C APIs such as `printf()`, `puts()`, `getchar()`,
`fgets()`, `malloc()`, and `free()` without including ATOM-specific headers.

For example, this standard C program can run unchanged on both a desktop operating
system and ATOM:

```c
#include <stdio.h>

int main(void)
{
    char buffer[128];

    while (fgets(buffer, sizeof(buffer), stdin))
    {
        printf("echo: %s", buffer);
    }
}
```

On ATOM, the board initialization configures UART0 as the default console and
connects it to the C standard streams. Therefore, stdin, stdout, printf(),
getchar(), puts(), and fgets() are available immediately after startup.

The runtime integration provides the low-level bindings required by newlib, including:

- Memory operations:
  - `memcpy`
  - `memset`
  - `memmove`
  - `memcmp`

- Formatted output:
  - `printf`
  - `snprintf`

- Dynamic memory management:
  - `malloc`
  - `calloc`
  - `realloc`
  - `free`

These facilities are provided without requiring an underlying operating system.

The ATOM kernel itself does not depend on dynamic allocation. Heap usage remains
an application-level feature.

---

# Building

## Requirements

- ARM GNU Toolchain
- CMake 3.24+
- Ninja (or Make)
- OpenOCD (optional)

## Build

```bash
git clone https://github.com/<user>/atom
cd atom

mkdir build
cd build

cmake \
    -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release ..

ninja
```

The generated executable is

```
atom
```

or

```
atom.elf
```

Create a UF2 image:

```bash
../tools/elf2uf2 atom -o atom.uf2
```

Copy the UF2 onto the Pico while it is in BOOTSEL mode.

---

# Architecture

ATOM implements a lightweight symmetric multiprocessing scheduler.

Both Cortex-M0+ cores execute application threads concurrently.

```
                Shared Scheduler

                 Ready Queue
             Running / Ready Threads
               Synchronization
                      │
             scheduler_spinlock
          ┌───────────┴───────────┐
          │                       │
          ▼                       ▼
      +---------+             +---------+
      | Core 0  |             | Core 1  |
      +---------+             +---------+
      | SysTick |             | SysTick |
      | PendSV  |             | PendSV  |
      +---------+             +---------+
```

Features:

- Shared thread pool
- Thread migration
- Independent context switching
- Hardware spinlock protection
- Core 0 maintains global system time

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
├── doc/
├── tools/
└── CMakeLists.txt
```

---

# Concurrency Primitives

| Primitive | Description |
|------------|-------------|
| Threads | Lightweight execution contexts |
| Mutex | Recursive mutual exclusion |
| Semaphore | Counting synchronization |
| Condition Variable | Wait/signal synchronization |
| Spinlock | Hardware-backed cross-core locking |
| Deferred Task | Delayed or periodic callbacks |
| Interrupt Control | Scoped interrupt masking |

---

# Peripheral Drivers

## GPIO

- Input/output
- Pull-up/down
- Read/write
- Toggle

## UART

- UART0
- UART1
- Configurable baud rate
- Blocking API

---

# Documentation

For implementation details, see the documents in the `doc/` directory.

Generate the complete API reference with Doxygen:

```bash
ninja doc
```

---

# Debugging

Start OpenOCD:

```bash
openocd -f openocd_pico.cfg
```

Connect using GDB:

```gdb
arm-none-eabi-gdb build/atom

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

---

# Limitations

Current limitations include:

- FIFO scheduler (no priorities)
- Shared vector table
- Core 0 owns the global time base
- No memory protection
- No userspace/kernel separation
- Application-managed thread stacks

---

# Contributing

Contributions are welcome.

Please:

1. Fork the repository.
2. Create a feature branch.
3. Add tests where appropriate.
4. Submit a pull request.

---

# License

See the LICENSE file.

---

# Acknowledgments

ATOM draws inspiration from established embedded kernels such as FreeRTOS and Zephyr while remaining an independent implementation.

---

# References

- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [ARM Cortex-M0+ Generic User Guide](https://developer.arm.com/documentation/du0621/latest/)
- [Raspberry Pi Pico Documentation](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html)