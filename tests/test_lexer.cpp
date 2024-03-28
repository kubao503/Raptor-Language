#include <gtest/gtest.h>

#include "errors.hpp"
#include "lexer.hpp"

class LexerTest : public testing::Test {
   protected:
    void SetUp(const std::string& input) {
        stream_ = std::istringstream(input);
        source_ = std::make_unique<Source>(stream_);
        lexer_ = std::make_unique<Lexer>(*source_);
    }

    std::istringstream stream_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<Lexer> lexer_;
};

TEST_F(LexerTest, getToken_bool_const) {
    SetUp("true false");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid type";
    EXPECT_EQ(std::get<bool>(token.value), true) << "Invalid value";

    token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid type";
    EXPECT_EQ(std::get<bool>(token.value), false) << "Invalid value";
}

TEST_F(LexerTest, getToken_while) {
    SetUp("while While");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::WHILE_KW) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value))
        << "Keyword token should not include value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ID)
        << "\"While\" is not a valid keyword";
}

TEST_F(LexerTest, getToken_id) {
    SetUp("valid_identifier_123");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::ID) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<std::string>(token.value))
        << "Invalid type of value";
    EXPECT_EQ(std::get<std::string>(token.value), "valid_identifier_123")
        << "Invalid value";
}

TEST_F(LexerTest, getToken_int) {
    SetUp("1234");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::INT_CONST) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<unsigned int>(token.value))
        << "Invalid type of value";
    EXPECT_EQ(std::get<unsigned int>(token.value), 1234) << "Invalid value";
}

TEST_F(LexerTest, getToken_int_with_leading_zero) {
    SetUp("01234");

    EXPECT_EQ(std::get<integral_t>(lexer_->getToken().value), 0) << "First part of int";

    EXPECT_EQ(std::get<integral_t>(lexer_->getToken().value), 1234)
        << "Second part of int";
}

TEST_F(LexerTest, getToken_int_overflow) {
    SetUp("4294967295 4294967296");

    EXPECT_NO_THROW(lexer_->getToken()) << "Just at the max";
    EXPECT_THROW(lexer_->getToken(), NumericOverflow) << "Max exceeded";
}

TEST_F(LexerTest, getToken_float) {
    SetUp("12.125");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::FLOAT_CONST) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<float>(token.value)) << "Invalid type of value";
    EXPECT_EQ(std::get<float>(token.value), 12.125f) << "Invalid value";
}

class LexerTestParam : public LexerTest,
                       public testing::WithParamInterface<std::string> {};

TEST_P(LexerTestParam, getToken_invalid_float) {
    SetUp(GetParam());

    EXPECT_THROW(lexer_->getToken(), InvalidFloat);
}

INSTANTIATE_TEST_SUITE_P(InvalidFloat, LexerTestParam, testing::Values("1..125", "1."));

TEST_F(LexerTest, getToken_float_overflow) {
    SetUp("0.4294967295  0.4294967296");

    EXPECT_NO_THROW(lexer_->getToken()) << "Just at the max";
    EXPECT_THROW(lexer_->getToken(), NumericOverflow) << "Max exceeded";
}

TEST_F(LexerTest, getToken_invalid) {
    SetUp("&324");

    EXPECT_THROW(lexer_->getToken(), InvalidToken);
}

TEST_F(LexerTest, getToken_empty_source) {
    SetUp("");

    for (int i = 0; i < 3; ++i) {
        auto token = lexer_->getToken();

        EXPECT_EQ(token.type, Token::Type::ETX) << "Invalid token type";
        EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value))
            << "Invalid type of token value";
    }
}

TEST_F(LexerTest, getToken_leading_white_space) {
    SetUp("   \t \n \r\n true");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::BOOL_CONST) << "Invalid token type";
    EXPECT_TRUE(std::holds_alternative<bool>(token.value))
        << "Invalid type of token value";
    EXPECT_EQ(std::get<bool>(token.value), true) << "Invalid token value";
}

TEST_F(LexerTest, getToken_twice) {
    SetUp("  while    return ");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::WHILE_KW) << "Invalid token type";

    token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::RETURN_KW) << "Invalid token type";
}

TEST_F(LexerTest, getToken_operators) {
    SetUp("< <= > >= = == != ; , . + - * / ( ) { } !");

    EXPECT_EQ(lexer_->getToken().type, Token::Type::LT_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::LTE_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::GT_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::GTE_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::ASGN_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::EQ_OP) << "Invalid token type";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::NEQ_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::SEMI) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::CMA) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::DOT) << "Invalid token type";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ADD_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::MIN_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::MULT_OP) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::DIV_OP) << "Invalid token type";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::L_PAR) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::R_PAR) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::L_C_BR) << "Invalid token type";
    EXPECT_EQ(lexer_->getToken().type, Token::Type::R_C_BR) << "Invalid token type";

    EXPECT_THROW(lexer_->getToken(), InvalidToken) << "Single ! is invalid";
}

TEST_F(LexerTest, getToken_str_const) {
    SetUp(" \"\\\"lama \\nma \\\\ delfina\\\"\" ");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::STR_CONST) << "Invalid token type";
    EXPECT_EQ(std::get<std::string>(token.value), "\"lama \nma \\ delfina\"")
        << "Invalid token value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid token type";
}

TEST_F(LexerTest, getToken_not_terminated_str_const) {
    SetUp("  \"no ending quotation mark");

    EXPECT_THROW(lexer_->getToken(), NotTerminatedStrConst)
        << "Str const without ending quotation mark";
}

TEST_F(LexerTest, getToken_backslash_at_the_end_of_file) {
    SetUp(R"("abc\)");

    EXPECT_THROW(lexer_->getToken(), NotTerminatedStrConst)
        << "Non-terminated string is more important than backslash";
}

TEST_F(LexerTest, getToken_escaping_wrong_char) {
    SetUp(" \"\\a\" ");

    EXPECT_THROW(lexer_->getToken(), NonEscapableChar) << "cannot escape char 'a'";
}

TEST_F(LexerTest, getToken_comment) {
    SetUp("   # int # \" 12 \" \n new line");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::CMT) << "Invalid token type";
    EXPECT_EQ(std::get<std::string>(token.value), " int # \" 12 \" ");
}

TEST_F(LexerTest, getToken_token_position) {
    SetUp("int void \nwhile");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.position.line, 1);
    EXPECT_EQ(token.position.column, 1);

    token = lexer_->getToken();
    EXPECT_EQ(token.position.line, 1);
    EXPECT_EQ(token.position.column, 5);

    token = lexer_->getToken();
    EXPECT_EQ(token.position.line, 2);
    EXPECT_EQ(token.position.column, 1);
}
