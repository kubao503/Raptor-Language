add_subdirectory(lexer)
add_subdirectory(parser)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(parser PUBLIC lexer)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE lexer
    PRIVATE parser
)
