add_library(
    parser
    parser.cpp
)

target_include_directories(parser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(parser PUBLIC lexer)
