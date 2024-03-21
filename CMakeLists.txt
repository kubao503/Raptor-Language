cmake_minimum_required(VERSION 3.14)
project(interpreter)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(compiler_flags INTERFACE)
target_compile_options(compiler_flags INTERFACE -Wall -Wextra -Werror)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(compiler_flags INTERFACE -g)
endif()


enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
