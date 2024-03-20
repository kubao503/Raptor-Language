#include <gtest/gtest.h>

#include "lexer.hpp"

TEST(lexer, getToken_bool) {
    std::istringstream stream("true");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid type";
    EXPECT_TRUE(std::holds_alternative<bool>(token.value)) << "Invalid type of value";
    EXPECT_EQ(std::get<bool>(token.value), true) << "Invalid value";
    EXPECT_EQ(token.position.line, 1) << "Invalid line";
}

TEST(lexer, getToken_while) {
    std::istringstream stream("while");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::WHILE_KW) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value)) << "Keyword token should not include value";
}

TEST(lexer, getToken_id) {
    std::istringstream stream("valid_identifier_123");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::ID) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<std::string>(token.value)) << "Invalid type of value";
    EXPECT_EQ(std::get<std::string>(token.value), "valid_identifier_123") << "Invalid value";
}

TEST(lexer, getToken_int) {
    std::istringstream stream("1234");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::INT_CONST) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<unsigned int>(token.value)) << "Invalid type of value";
    EXPECT_EQ(std::get<unsigned int>(token.value), 1234) << "Invalid value";
}

TEST(lexer, getToken_int_with_leading_zeros) {
    std::istringstream stream("0001234");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    EXPECT_THROW(lexer.getToken(), InvalidToken) << "Leading zeros are forbidden";
}

TEST(lexer, getToken_float) {
    std::istringstream stream("12.125");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::FLOAT_CONST) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<float>(token.value)) << "Invalid type of value";
    EXPECT_EQ(std::get<float>(token.value), 12.125f) << "Invalid value";
}

TEST(lexer, getToken_invalid_float) {
    std::string invalidFloats[] = {"1..125", "1.", ".5"};

    for (auto s : invalidFloats) {
        std::istringstream stream(s);
        auto source = Source(stream);
        auto lexer = Lexer(source);

        EXPECT_THROW(lexer.getToken(), InvalidToken);
    }
}

TEST(lexer, getToken_invalid) {
    std::istringstream stream("&324");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    EXPECT_THROW(lexer.getToken(), InvalidToken);
}

TEST(lexer, getToken_empty_source) {
    std::istringstream stream("");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    for (int i = 0; i < 3; ++i) {
        auto token = lexer.getToken();

        EXPECT_EQ(token.type, Token::Type::ETX) << "Invalid token type";
        EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value)) << "Invalid type of token value";
    }
}

TEST(lexer, getToken_leading_white_space) {
    std::istringstream stream("   \t \n \r\n true");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<bool>(token.value)) << "Invalid type of token value";
    EXPECT_EQ(std::get<bool>(token.value), true) << "Invalid token value";
    EXPECT_EQ(token.position.line, 3) << "Invalid line";
}

TEST(lexer, getToken_twice) {
    std::istringstream stream("  while    return ");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();
    EXPECT_EQ(token.type, Token::Type::WHILE_KW) << "Invalid token type";

    token = lexer.getToken();
    EXPECT_EQ(token.type, Token::Type::RETURN_KW) << "Invalid token type";
}
