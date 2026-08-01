# ------------------------------------------------------------------------------
# Global C settings
# ------------------------------------------------------------------------------
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS_DEBUG "-O2 -g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-Os -flto")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR}/build
)

# ------------------------------------------------------------------------------
# ATOM library
# ------------------------------------------------------------------------------
add_library(atom STATIC)


target_sources(atom PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/util/log.c
        ${CMAKE_CURRENT_SOURCE_DIR}/util/duration.c
        ${CMAKE_CURRENT_SOURCE_DIR}/util/collection/sorted_list.c
)

# ------------------------------------------------------------------------------
# Public headers
# ------------------------------------------------------------------------------
target_include_directories(atom PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# ------------------------------------------------------------------------------
# Compiler options
# ------------------------------------------------------------------------------
target_compile_options(atom PRIVATE
        -fstrict-volatile-bitfields
        -ffunction-sections
        -fdata-sections

        -Wall
        -Wextra
        -Wcast-align
        -Wconversion
        -Wsign-conversion
        -Wshadow

        -ffreestanding
)

# ------------------------------------------------------------------------------
# Architecture
# ------------------------------------------------------------------------------
add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/arch/rp2040
)