set(LEXER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lexer)

add_library(
    lexer
    ${LEXER_DIR}/lexer.cpp
    ${LEXER_DIR}/token.cpp
    ${LEXER_DIR}/errors.cpp
)

target_include_directories(lexer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lexer)

target_link_libraries(
    lexer
    PUBLIC compiler_flags
    PUBLIC magic_enum::magic_enum
    PUBLIC boost::boost
)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC compiler_flags
    PRIVATE lexer
)
