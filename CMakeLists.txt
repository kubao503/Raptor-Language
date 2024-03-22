cmake_minimum_required(VERSION 3.14)
project(interpreter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(magic_enum REQUIRED)


add_library(compiler_flags INTERFACE)
target_compile_options(compiler_flags INTERFACE -Wall -Wextra -Werror -pedantic)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(compiler_flags INTERFACE -g)
endif()


enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
