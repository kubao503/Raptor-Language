# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(
    tests
    test_lexer.cpp
    test_filter.cpp
    test_source.cpp
)

target_link_libraries(
    tests
    PRIVATE gtest::gtest
    PRIVATE lexer
)

include(GoogleTest)
gtest_discover_tests(tests)
