# ------------------------------------------------------------------------------
# CPU configuration
# ------------------------------------------------------------------------------
set(CPU_OPTIONS
        -mthumb
        -mcpu=cortex-m0plus
        -mfloat-abi=soft
)

set(CPU_FREQUENCY 125000000 CACHE STRING "CPU frequency in Hz")

set(UART_BAUD_RATE 115200 CACHE STRING "Default UART baud rate")

# ------------------------------------------------------------------------------
# Generate configuration
# ------------------------------------------------------------------------------
configure_file(
        "${CMAKE_SOURCE_DIR}/include/atom_config.h.in"
        "${CMAKE_SOURCE_DIR}/include/atom_config.h"
        @ONLY
)

# ------------------------------------------------------------------------------
# RP2040 sources
# ------------------------------------------------------------------------------
target_sources(${PROJECT_NAME} PRIVATE

        "${CMAKE_CURRENT_SOURCE_DIR}/libc.c"

        "${CMAKE_SOURCE_DIR}/src/main/libc.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/boot/startup.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/boot/checksum.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/boot/board.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/io/uart.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/io/gpio.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/interrupts.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/thread.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/spinlock.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/scheduler.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/mutex.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/semaphore.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/condition_variable.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/deferred_task.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/concurrent/event_flags.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/util/time/timestamp.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/system/pll.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/clocks.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/cpu.c"
)

# ------------------------------------------------------------------------------
# Private RP2040 headers
# ------------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ------------------------------------------------------------------------------
# CPU flags
# ------------------------------------------------------------------------------
target_compile_options(${PROJECT_NAME} PRIVATE ${CPU_OPTIONS})

