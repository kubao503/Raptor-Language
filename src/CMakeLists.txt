add_library(
    lexer
    lexer.cpp
    token.cpp
)

target_include_directories(lexer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lexer PUBLIC compiler_flags magic_enum::magic_enum)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC compiler_flags
    PRIVATE lexer
)
