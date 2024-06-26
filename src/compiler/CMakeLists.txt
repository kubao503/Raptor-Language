find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_library(
    compiler
    IRGenerator.cpp
    ExprIRGenerator.cpp
    compiler.cpp
)

add_library(print print.cpp)

target_include_directories(compiler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(compiler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "LLVM Libraries: ${llvm_libs}")

target_link_libraries(
    compiler
    PUBLIC parser
    LLVM-14
)