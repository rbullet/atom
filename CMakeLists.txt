cmake_minimum_required(VERSION 3.24)

project(atom VERSION 0.0.1 LANGUAGES C ASM)

# ------------------------------------------------------------------------------
# ATOM static library
# ------------------------------------------------------------------------------
add_subdirectory(src/main)

# ------------------------------------------------------------------------------
# Examples
# ------------------------------------------------------------------------------
add_subdirectory(examples/00_developer_playground)
add_subdirectory(examples/01_hello_world)
add_subdirectory(examples/02_blink)
add_subdirectory(examples/03_stdin_stdout)
add_subdirectory(examples/04_assert)
add_subdirectory(examples/05_logging)
add_subdirectory(examples/06_timer)
add_subdirectory(examples/07_thread)
add_subdirectory(examples/08_thread_result)
add_subdirectory(examples/09_mutex)
add_subdirectory(examples/10_semaphore)
add_subdirectory(examples/11_condition_variable)
add_subdirectory(examples/12_event_flags)

# ------------------------------------------------------------------------------
# Doxygen documentation
# ------------------------------------------------------------------------------
find_package(Doxygen)

if (Doxygen_FOUND)
    set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile")
    set(DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile")

    configure_file(
            "${DOXYGEN_IN}"
            "${DOXYGEN_OUT}"
            @ONLY
    )

    add_custom_target(doc ALL
            COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
    )

    add_custom_command(
            TARGET doc POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_BINARY_DIR}/doc/html/index.html"
            "${CMAKE_SOURCE_DIR}/doc/index.html"
    )

endif()