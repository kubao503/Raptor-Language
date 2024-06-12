# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(
    tests
    test_source.cpp
    test_lexer.cpp
    test_filter.cpp
    test_stmt_parsing.cpp
    test_expr_parsing.cpp
    test_interpreter.cpp
    acceptance_tests.cpp
)

target_link_libraries(
    tests
    PRIVATE gtest::gtest
    PRIVATE lexer
    PRIVATE parser
    PRIVATE interpreter
)

include(GoogleTest)
gtest_discover_tests(tests)
