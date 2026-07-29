set(CPU_OPTIONS
        -mthumb
        -mcpu=cortex-m0plus
        -mfloat-abi=soft
)

# CPU configuration
set(CPU_FREQUENCY 125000000 CACHE STRING "CPU frequency in Hz")  # 125 MHz
set(UART_BAUD_RATE 115200 CACHE STRING "Default UART baud rate")

# Generate a header
set(GENERATED_INCLUDE_DIR ${CMAKE_BINARY_DIR}/generated/include)
file(MAKE_DIRECTORY ${GENERATED_INCLUDE_DIR})
configure_file(
        ${CMAKE_SOURCE_DIR}/include/atom_config.h.in
        ${GENERATED_INCLUDE_DIR}/atom_config.h
        @ONLY
)

target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/libc.c
        ${CMAKE_SOURCE_DIR}/src/main/libc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/boot/startup.c
        ${CMAKE_CURRENT_SOURCE_DIR}/boot/checksum.c
        ${CMAKE_CURRENT_SOURCE_DIR}/boot/board.c
        ${CMAKE_CURRENT_SOURCE_DIR}/io/uart.c
        ${CMAKE_CURRENT_SOURCE_DIR}/io/gpio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrent/interrupts.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrent/thread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrent/spinlock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrent/scheduler.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrent/mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrent/semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrent/condition_variable.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrent/deferred_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/util/time/timestamp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/system/pll.c
        ${CMAKE_CURRENT_SOURCE_DIR}/system/clocks.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${GENERATED_INCLUDE_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE
        ${CPU_OPTIONS}
)

target_link_options(${PROJECT_NAME} PRIVATE
        -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
        ${CPU_OPTIONS}
        -Wl,--no-whole-archive
        --specs=nano.specs
)