cmake_minimum_required(VERSION 3.14)
project(interpreter)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(compiler_flags INTERFACE)
target_compile_options(compiler_flags INTERFACE -Wall -Wextra -Werror)

add_subdirectory(src)
add_subdirectory(tests)
