add_library(
    parse_tree
    expressions.cpp
    statements.cpp
    printer.cpp
)

target_include_directories(parse_tree INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(parse_tree PUBLIC lexer)
