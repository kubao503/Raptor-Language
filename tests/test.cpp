#include <gtest/gtest.h>

#include "lexer.hpp"

TEST(lexer, getToken) {
    std::istringstream stream("true");
    auto lexer = Lexer(stream);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::BOOL_CONST);
    EXPECT_TRUE(std::holds_alternative<bool>(token.value));
    EXPECT_EQ(std::get<bool>(token.value), true);
}
