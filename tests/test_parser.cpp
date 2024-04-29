#include <gtest/gtest.h>

#include "fixtures.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "parser_errors.hpp"

class ParserTest : public testing::Test {
   protected:
    template <typename T>
    void SetUp(std::initializer_list<T> seq) {
        lexer_ = std::make_unique<FakeLexer>(seq);
        parser_ = std::make_unique<Parser>(*lexer_);
    }

    std::unique_ptr<FakeLexer> lexer_;
    std::unique_ptr<Parser> parser_;
};

TEST_F(ParserTest, parseProgram_empty) {
    SetUp({Token::Type::ETX});

    auto prog = parser_->parseProgram();
    EXPECT_EQ(prog.statements.size(), 0);
}

TEST_F(ParserTest, parseProgram_if_stmt_missing_left_par) {
    SetUp({Token::Type::IF_KW});

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(ParserTest, parseProgram_if_stmt) {
    SetUp({Token::Type::IF_KW, Token::Type::L_PAR});

    auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    EXPECT_TRUE(std::holds_alternative<IfStatement>(prog.statements.at(0)));
}

TEST_F(ParserTest, parse_func_def) {
    SetUp<Token>({{Token::Type::INT_KW, {}, {}},
                  {Token::Type::ID, "add_one", {}},
                  {Token::Type::L_PAR, {}, {}}});

    auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
}
