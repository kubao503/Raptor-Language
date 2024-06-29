add_library(
    interpreter
    interpreter.cpp
    expr_interpreter.cpp
    call_context.cpp
    scope.cpp
    value_obj.cpp
    interpreter_errors.cpp
)

target_include_directories(interpreter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    interpreter
    PUBLIC parser
)
