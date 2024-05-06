add_library(
    parser
    parser.cpp
    statements.cpp
    expressions.cpp
    printer.cpp
)

target_include_directories(parser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(parser PUBLIC lexer)
