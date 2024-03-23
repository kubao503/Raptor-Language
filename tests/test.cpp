#include <gtest/gtest.h>

#include "lexer.hpp"

TEST(lexer, getToken_bool) {
    std::istringstream stream("true false");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();
    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid type";
    EXPECT_EQ(std::get<bool>(token.value), true) << "Invalid value";

    token = lexer.getToken();
    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid type";
    EXPECT_EQ(std::get<bool>(token.value), false) << "Invalid value";
}

TEST(lexer, getToken_while) {
    std::istringstream stream("while");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();

    EXPECT_EQ(token.type, Token::Type::WHILE_KW) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value))
        << "Keyword token should not include value";
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

TEST(lexer, getToken_int_with_leading_zero) {
    std::istringstream stream("01234");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    EXPECT_EQ(std::get<integral_t>(lexer.getToken().value), 0) << "First part of int";

    EXPECT_EQ(std::get<integral_t>(lexer.getToken().value), 1234) << "Second part of int";
}

TEST(lexer, getToken_int_overflow) {
    std::istringstream stream("4294967295 4294967296");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    EXPECT_NO_THROW(lexer.getToken()) << "Just at the max";
    EXPECT_THROW(lexer.getToken(), IntOverflow) << "Max exceeded";
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
    std::string invalidFloats[] = {"1..125", "1."};

    for (auto s : invalidFloats) {
        std::istringstream stream(s);
        auto source = Source(stream);
        auto lexer = Lexer(source);

        EXPECT_THROW(lexer.getToken(), InvalidToken);
    }
}

TEST(lexer, getToken_float_overflow) {
    std::istringstream stream("0.4294967295  0.4294967296");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    EXPECT_NO_THROW(lexer.getToken()) << "Just at the max";
    EXPECT_THROW(lexer.getToken(), FloatOverflow) << "Max exceeded";
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
        EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value))
            << "Invalid type of token value";
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

TEST(lexer, getToken_operators) {
    std::istringstream stream("< <= > >= = == != ; , . + - * / ( ) { } !");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    EXPECT_EQ(lexer.getToken().type, Token::Type::LT_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::LTE_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::GT_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::GTE_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::ASGN_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::EQ_OP) << "Invalid token type";

    EXPECT_EQ(lexer.getToken().type, Token::Type::NEQ_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::SEMI) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::CMA) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::DOT) << "Invalid token type";

    EXPECT_EQ(lexer.getToken().type, Token::Type::ADD_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::MIN_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::MULT_OP) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::DIV_OP) << "Invalid token type";

    EXPECT_EQ(lexer.getToken().type, Token::Type::L_PAR) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::R_PAR) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::L_C_BR) << "Invalid token type";
    EXPECT_EQ(lexer.getToken().type, Token::Type::R_C_BR) << "Invalid token type";

    EXPECT_THROW(lexer.getToken(), InvalidToken) << "Single ! is invalid";
}

TEST(lexer, getToken_str_const) {
    std::istringstream stream("  \"\\\"lama \\nma delfina\\\"\" ");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();
    EXPECT_EQ(token.type, Token::Type::STR_CONST) << "Invalid token type";
    EXPECT_EQ(std::get<std::string>(token.value), "\"lama \nma delfina\"") << "Invalid token value";

    EXPECT_EQ(lexer.getToken().type, Token::Type::ETX) << "Invalid token type";
}

TEST(lexer, getToken_invalid_str) {
    std::istringstream stream("  \"no ending quotation mark");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    EXPECT_THROW(std::get<std::string>(lexer.getToken().value), InvalidToken)
        << "Str const without ending quotation mark";
}

TEST(lexer, getToken_comment) {
    std::istringstream stream("   # int # \" 12 \" \n new line");
    auto source = Source(stream);
    auto lexer = Lexer(source);

    auto token = lexer.getToken();
    EXPECT_EQ(token.type, Token::Type::CMT) << "Invalid token type";
    EXPECT_EQ(std::get<std::string>(token.value), " int # \" 12 \" ");
}
