# arm-none-eabi-toolchain.cmake

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Path to your ARM toolchain
#set(TOOLCHAIN_PATH "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.3 rel1/bin")
set(TOOLCHAIN_PATH "/usr/bin")

set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-gcc")

#set(CMAKE_SYSROOT "/usr/arm-none-eabi")

# Prevent CMake from using host headers/libs
set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# No system libraries — bare-metal target
set(CMAKE_EXE_LINKER_FLAGS_INIT "")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)