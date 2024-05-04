#ifndef PARSER_FIXTURE_H
#define PARSER_FIXTURE_H

#include "fixtures.hpp"
#include "parser.hpp"

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

#endif
