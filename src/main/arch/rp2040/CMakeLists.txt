# CPU configuration
set(CPU_OPTIONS
        -mthumb
        -mcpu=cortex-m0plus
        -mfloat-abi=soft
)

set(CPU_FREQUENCY
        125000000
        CACHE STRING
        "CPU frequency in Hz"
)

set(UART_BAUD_RATE
        115200
        CACHE STRING
        "Default UART baud rate"
)

# Generate configuration
configure_file(
        ${CMAKE_SOURCE_DIR}/include/atom_config.h.in
        ${CMAKE_SOURCE_DIR}/include/atom_config.h
        @ONLY
)

# RP2040 sources
target_sources(atom PRIVATE

        libc.c

        ${CMAKE_SOURCE_DIR}/src/main/libc.c

        boot/startup.c
        boot/checksum.c
        boot/board.c

        io/uart.c
        io/gpio.c

        concurrent/interrupts.c
        concurrent/thread.c
        concurrent/spinlock.c
        concurrent/scheduler.c
        concurrent/mutex.c
        concurrent/semaphore.c
        concurrent/condition_variable.c
        concurrent/deferred_task.c

        util/time/timestamp.c

        system/pll.c
        system/clocks.c
)

# Private RP2040 headers
target_include_directories(atom PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# CPU flags
target_compile_options(atom PRIVATE
        ${CPU_OPTIONS}
)

