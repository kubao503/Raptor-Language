#include <gtest/gtest.h>

#include "lexer.hpp"

TEST(lexer, getToken_bool) {
    std::istringstream stream("true");
    auto lexer = Lexer(stream);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid type";
    EXPECT_TRUE(std::holds_alternative<bool>(token.value)) << "Invalid type of value";
    EXPECT_EQ(std::get<bool>(token.value), true) << "Invalid value";
    EXPECT_EQ(token.position.line, 1) << "Invalid line";
}

TEST(lexer, getToken_while) {
    std::istringstream stream("while");
    auto lexer = Lexer(stream);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::WHILE_KW) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value)) << "Keyword token should not include value";
}

TEST(lexer, getToken_id) {
    std::istringstream stream("identifier");
    auto lexer = Lexer(stream);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::ID) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<std::string>(token.value)) << "Invalid type of value";
    EXPECT_EQ(std::get<std::string>(token.value), "identifier") << "Invalid value";
}

TEST(lexer, getToken_invalid) {
    std::istringstream stream("&324");
    auto lexer = Lexer(stream);

    EXPECT_THROW(lexer.getToken(), InvalidToken);
}

TEST(lexer, getToken_empty_stream) {
    std::istringstream stream("");
    auto lexer = Lexer(stream);

    EXPECT_THROW(lexer.getToken(), InvalidToken);
}

TEST(lexer, getToken_leading_white_space) {
    std::istringstream stream("   \t \n \r\n true");
    auto lexer = Lexer(stream);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<bool>(token.value)) << "Invalid type of token value";
    EXPECT_EQ(std::get<bool>(token.value), true) << "Invalid token value";
    EXPECT_EQ(token.position.line, 3) << "Invalid line";
}
