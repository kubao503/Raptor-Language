cmake_minimum_required(VERSION 3.14)
project(interpreter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fprofile-arcs -ftest-coverage -fsanitize=undefined")

find_package(magic_enum REQUIRED)
find_package(GTest REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
