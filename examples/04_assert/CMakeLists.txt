# ------------------------------------------------------------------------------
# Example name
# ------------------------------------------------------------------------------
get_filename_component(EXAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# ------------------------------------------------------------------------------
# Linker script
# ------------------------------------------------------------------------------
# Copy the architecture linker script into the example directory.
# This keeps the example self-contained and allows it to be copied
# as a standalone ATOM project.
configure_file(
        ${CMAKE_SOURCE_DIR}/src/main/arch/rp2040/linker.ld
        ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
        COPYONLY
)

# ------------------------------------------------------------------------------
# CPU flags
# ------------------------------------------------------------------------------
set(CPU_OPTIONS
        -mthumb
        -mcpu=cortex-m0plus
        -mfloat-abi=soft
)

set(CMAKE_C_FLAGS_DEBUG "-O2 -g -DDEBUG")

# ------------------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------------------
add_executable(${EXAMPLE_NAME}
        main.c
)

# ------------------------------------------------------------------------------
# Output directory
# ------------------------------------------------------------------------------
set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
)

# ------------------------------------------------------------------------------
# Compile options
# ------------------------------------------------------------------------------
target_compile_options(${EXAMPLE_NAME}
        PRIVATE
        ${CPU_OPTIONS}
)

# ------------------------------------------------------------------------------
# Link libraries
# ------------------------------------------------------------------------------
target_link_libraries(${EXAMPLE_NAME}
        PRIVATE
        "-Wl,--whole-archive"
        atom
        "-Wl,--no-whole-archive"
)

# ------------------------------------------------------------------------------
# Linker options
# ------------------------------------------------------------------------------
target_link_options(${EXAMPLE_NAME}
        PRIVATE
        -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
        ${CPU_OPTIONS}
        --specs=nano.specs
)