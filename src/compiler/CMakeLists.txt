add_library(
    compiler
    ir_generator.cpp
)

target_include_directories(compiler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    compiler
    PUBLIC parser
)