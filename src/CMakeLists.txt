add_subdirectory(utils)
add_subdirectory(errors)
add_subdirectory(lexer)
add_subdirectory(parse_tree)
add_subdirectory(parser)
add_subdirectory(interpreter)
add_subdirectory(compiler)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE lexer
    PRIVATE parser
    PRIVATE interpreter
    PRIVATE compiler
)
