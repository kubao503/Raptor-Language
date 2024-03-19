#include <gtest/gtest.h>

#include "lexer.hpp"

TEST(HelloTest, BasicAssertions) {
    std::string s = "cake";
    std::istringstream stream(s);
    EXPECT_EQ(getToken(stream), 'c');
}

TEST(HelloTest, Other) {
    EXPECT_EQ(1, 1);
}
