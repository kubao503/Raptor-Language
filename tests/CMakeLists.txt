# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(
    tests
    test_source.cpp
    test_lexer.cpp
    test_filter.cpp
    test_parser.cpp
)

target_link_libraries(
    tests
    PRIVATE gtest::gtest
    PRIVATE lexer
    PRIVATE parser
)

include(GoogleTest)
gtest_discover_tests(tests)
