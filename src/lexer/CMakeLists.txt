add_library(
    lexer
    lexer.cpp
    token.cpp
)

target_include_directories(lexer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    lexer
    PUBLIC magic_enum::magic_enum
    PUBLIC errors
    PUBLIC utils
)
