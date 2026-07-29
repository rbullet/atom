cmake_minimum_required(VERSION 3.24)

# ------------------------------------------------------------------------------
# Project configuration
# ------------------------------------------------------------------------------

project(atom VERSION 0.0.1 LANGUAGES C ASM)

add_subdirectory(src/main)

find_package(Doxygen)

if (Doxygen_FOUND)
    set(DOXYGEN_IN doc/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
endif ()


