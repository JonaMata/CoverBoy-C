cmake_minimum_required(VERSION 3.30)
project(CoverBoy_C)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(triple aarch64-arm-none-eabi)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

file(GLOB_RECURSE RGB_SRC_FILES rpi-rgb-led-matrix/lib/.cc lib/.c)
file(GLOB_RECURSE RGB_HEADER_FILES rpi-rgb-led-matrix/include/*.h)

add_executable(CoverBoy_C main.cpp ${RGB_SRC_FILES} ${RGB_HEADER_FILES})
