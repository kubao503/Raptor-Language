add_library(
    interpreter
    interpreter.cpp
    call_context.cpp
)

target_include_directories(interpreter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    interpreter
    PUBLIC parser
)
