#include <gtest/gtest.h>

#include "fixtures.hpp"
#include "lexer.hpp"
#include "parser.hpp"

class ParserTest : public testing::Test {
   protected:
    void SetUp(std::initializer_list<Token::Type> seq) {
        lexer_ = std::make_unique<FakeLexer>(seq);
        parser_ = std::make_unique<Parser>(*lexer_);
    }

    std::unique_ptr<FakeLexer> lexer_;
    std::unique_ptr<Parser> parser_;
};

TEST_F(ParserTest, parseProgram_empty) {
    SetUp({});

    auto prog = parser_->parseProgram();
    EXPECT_EQ(prog.statements.size(), 0);
}

TEST_F(ParserTest, parseProgram_if_stmt) {
    SetUp({Token::Type::IF_KW});

    auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    EXPECT_TRUE(std::holds_alternative<IfStatement>(prog.statements.at(0)));
}
