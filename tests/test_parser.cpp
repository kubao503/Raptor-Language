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

TEST_F(ParserTest, parseProgram) {
    SetUp({});

    parser_->parseProgram();
}
