#include <gtest/gtest.h>

#include "errors.hpp"
#include "lexer.hpp"
#include "types.hpp"

using TypeSequence = std::vector<Token::Type>;

class LexerTest : public testing::Test {
   protected:
    void SetUp(const std::string& input) {
        stream_ = std::istringstream(input);
        source_ = std::make_unique<Source>(&stream_);
        lexer_ = std::make_unique<Lexer>(source_.get());
    }

    std::istringstream stream_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<Lexer> lexer_;
};

TEST_F(LexerTest, getToken_true) {
    SetUp("true");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::TRUE_CONST) << "Invalid type";
    EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value)) << "Invalid value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

TEST_F(LexerTest, getToken_false) {
    SetUp("false");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::FALSE_CONST) << "Invalid type";
    EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value)) << "Invalid value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

TEST_F(LexerTest, getToken_while_keyword) {
    SetUp("while");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::WHILE_KW) << "Invalid type";
    EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value)) << "Invalid value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

TEST_F(LexerTest, getToken_id) {
    SetUp("valid_identifier_123");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::ID) << "Invalid type";
    EXPECT_EQ(std::get<std::string>(token.value), "valid_identifier_123")
        << "Invalid value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

TEST_F(LexerTest, getToken_int) {
    SetUp("1234");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::INT_CONST) << "Invalid type";
    EXPECT_EQ(std::get<integral_t>(token.value), 1234) << "Invalid value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

TEST_F(LexerTest, getToken_int_with_leading_zero) {
    SetUp("01234");

    EXPECT_EQ(std::get<integral_t>(lexer_->getToken().value), 0) << "First part of int";

    EXPECT_EQ(std::get<integral_t>(lexer_->getToken().value), 1234)
        << "Second part of int";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

TEST_F(LexerTest, getToken_int_max) {
    SetUp("4294967295");

    EXPECT_NO_THROW(lexer_->getToken()) << "Just at the max";
}

TEST_F(LexerTest, getToken_int_overflow) {
    SetUp("4294967296");

    EXPECT_THROW(lexer_->getToken(), NumericOverflow) << "Max exceeded";
}

TEST_F(LexerTest, getToken_float) {
    SetUp("12.125");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::FLOAT_CONST) << "Invalid type";
    EXPECT_EQ(std::get<float>(token.value), 12.125f) << "Invalid value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

class LexerFloatTest : public LexerTest,
                       public testing::WithParamInterface<std::string> {};

TEST_P(LexerFloatTest, getToken_invalid_float) {
    SetUp(GetParam());

    EXPECT_THROW(lexer_->getToken(), InvalidFloat);
}

INSTANTIATE_TEST_SUITE_P(InvalidFloat, LexerFloatTest, testing::Values("1..125", "1."));

TEST_F(LexerTest, getToken_float_max) {
    SetUp("0.4294967295");

    EXPECT_NO_THROW(lexer_->getToken()) << "Just at the max";
}

TEST_F(LexerTest, getToken_float_overflow) {
    SetUp("0.4294967296");

    EXPECT_THROW(lexer_->getToken(), NumericOverflow) << "Max exceeded";
}

TEST_F(LexerTest, getToken_invalid) {
    SetUp("&324");

    EXPECT_THROW(lexer_->getToken(), InvalidToken);
}

TEST_F(LexerTest, getToken_empty_source) {
    SetUp("");

    for (int i{0}; i < 5; ++i) {
        auto token = lexer_->getToken();
        EXPECT_EQ(token.type, Token::Type::ETX) << "Invalid type";
        EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value))
            << "Invalid value";
    }
}

TEST_F(LexerTest, getToken_leading_white_space) {
    SetUp("   \t \n \r\n true");

    auto token = lexer_->getToken();

    EXPECT_EQ(token.type, Token::Type::TRUE_CONST) << "Invalid type";
}

class LexerOperatorTest
    : public LexerTest,
      public testing::WithParamInterface<std::pair<std::string, Token::Type>> {};

TEST_P(LexerOperatorTest, getToken_operators) {
    SetUp(GetParam().first);

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, GetParam().second) << "Invalid type";
    EXPECT_TRUE(std::holds_alternative<std::monostate>(token.value)) << "Invalid value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

INSTANTIATE_TEST_SUITE_P(Operators, LexerOperatorTest,
                         testing::Values(std::make_pair("<", Token::Type::LT_OP),
                                         std::make_pair("<=", Token::Type::LTE_OP),
                                         std::make_pair(">", Token::Type::GT_OP),
                                         std::make_pair(">=", Token::Type::GTE_OP),
                                         std::make_pair("=", Token::Type::ASGN_OP),
                                         std::make_pair("==", Token::Type::EQ_OP),
                                         std::make_pair("!=", Token::Type::NEQ_OP),
                                         std::make_pair(";", Token::Type::SEMI),
                                         std::make_pair(",", Token::Type::CMA),
                                         std::make_pair(".", Token::Type::DOT),
                                         std::make_pair("+", Token::Type::ADD_OP),
                                         std::make_pair("-", Token::Type::MIN_OP),
                                         std::make_pair("*", Token::Type::MULT_OP),
                                         std::make_pair("/", Token::Type::DIV_OP),
                                         std::make_pair("(", Token::Type::L_PAR),
                                         std::make_pair(")", Token::Type::R_PAR),
                                         std::make_pair("{", Token::Type::L_C_BR),
                                         std::make_pair("}", Token::Type::R_C_BR)));

TEST_F(LexerTest, getToken_str_const) {
    SetUp(R"("\"lama \nma \\ delfina\"")");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::STR_CONST) << "Invalid type";
    EXPECT_EQ(std::get<std::string>(token.value), "\"lama \nma \\ delfina\"")
        << "Invalid token value";

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

TEST_F(LexerTest, getToken_not_terminated_str_const) {
    SetUp(R"("no ending quotation mark)");

    EXPECT_THROW(lexer_->getToken(), NotTerminatedStrConst)
        << "Str const without ending quotation mark";
}

TEST_F(LexerTest, getToken_backslash_at_the_end_of_file) {
    SetUp(R"("abc\)");

    EXPECT_THROW(lexer_->getToken(), NotTerminatedStrConst)
        << "Non-terminated string is more important than backslash";
}

TEST_F(LexerTest, getToken_escaping_wrong_char) {
    SetUp(R"("\a")");

    EXPECT_THROW(lexer_->getToken(), NonEscapableChar) << "cannot escape char 'a'";
}

TEST_F(LexerTest, getToken_comment) {
    SetUp(R"(# int 12 # "abc")");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::CMT) << "Invalid type";
    EXPECT_EQ(std::get<std::string>(token.value), R"( int 12 # "abc")");

    EXPECT_EQ(lexer_->getToken().type, Token::Type::ETX) << "Invalid type";
}

TEST_F(LexerTest, getToken_end_comment_at_new_line) {
    SetUp("# first line\n second line");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.type, Token::Type::CMT) << "Invalid type";
    EXPECT_EQ(std::get<std::string>(token.value), R"( first line)");
}

TEST_F(LexerTest, getToken_token_position_one_line) {
    SetUp("int void");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.position.line, 1);
    EXPECT_EQ(token.position.column, 1);

    token = lexer_->getToken();
    EXPECT_EQ(token.position.line, 1);
    EXPECT_EQ(token.position.column, 5);
}

TEST_F(LexerTest, getToken_token_position_two_lines) {
    SetUp("abc\ndef");

    auto token = lexer_->getToken();
    EXPECT_EQ(token.position.line, 1);
    EXPECT_EQ(token.position.column, 1);

    token = lexer_->getToken();
    EXPECT_EQ(token.position.line, 2);
    EXPECT_EQ(token.position.column, 1);
}

TEST_F(LexerTest, getToken_multiple_tokens) {
    std::string input{
        "void add_one_ref(ref int num) {"
        "    num = num + 1;"
        "}"};
    SetUp(input);

    TypeSequence seq{
        Token::Type::VOID_KW,   Token::Type::ID,     Token::Type::L_PAR,
        Token::Type::REF_KW,    Token::Type::INT_KW, Token::Type::ID,
        Token::Type::R_PAR,     Token::Type::L_C_BR, Token::Type::ID,
        Token::Type::ASGN_OP,   Token::Type::ID,     Token::Type::ADD_OP,
        Token::Type::INT_CONST, Token::Type::SEMI,   Token::Type::R_C_BR,
        Token::Type::ETX,
    };

    for (auto type : seq) EXPECT_EQ(lexer_->getToken().type, type) << "Invalid type";
}
