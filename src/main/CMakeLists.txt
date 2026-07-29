# ------------------------------------------------------------------------------
# Global C settings
# ------------------------------------------------------------------------------
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS_DEBUG "-O2 -g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-Os -flto")

# ------------------------------------------------------------------------------
# CPU & compiler options for RP2040
# ------------------------------------------------------------------------------
add_compile_options(
        -fstrict-volatile-bitfields
        -ffunction-sections
        -fdata-sections

        #-Werror
        -Wall
        -Wextra
        -Wcast-align
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wlogical-op
        -Wsuggest-final-types
        -Wsuggest-final-methods
        -ffreestanding
)

# ------------------------------------------------------------------------------
# Main executable
# ------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/src/main/main.c
        ${CMAKE_SOURCE_DIR}/src/main/util/log.c
        ${CMAKE_SOURCE_DIR}/src/main/util/assert.c
        ${CMAKE_SOURCE_DIR}/src/main/util/duration.c
        ${CMAKE_SOURCE_DIR}/src/main/util/collection/sorted_list.c
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_options(${PROJECT_NAME} PRIVATE
        -Wl,-Map=${PROJECT_NAME}.map
)

# ------------------------------------------------------------------------------
# Include RP2040 sources from child CMake
# ------------------------------------------------------------------------------
add_subdirectory(${CMAKE_SOURCE_DIR}/src/main/arch/rp2040)  # this sets RP2040_SOURCES variable