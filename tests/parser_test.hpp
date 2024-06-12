#ifndef PARSER_FIXTURE_H
#define PARSER_FIXTURE_H

#include "lexer.hpp"
#include "parser.hpp"

class ParserTest : public testing::Test {
   protected:
    void Init(std::string input) {
        stream_ = std::istringstream(input);
        source_ = std::make_unique<Source>(stream_);
        lexer_ = std::make_unique<Lexer>(*source_);
        parser_ = std::make_unique<Parser>(*lexer_);
    }

    template <typename Exception>
    void parseAndExpectThrowAt(Position position) {
        EXPECT_THROW(
            {
                try {
                    parser_->parseProgram();
                } catch (const Exception& e) {
                    EXPECT_EQ(e.getPosition().line, position.line);
                    EXPECT_EQ(e.getPosition().column, position.column);
                    throw;
                }
            },
            Exception);
    }

    std::istringstream stream_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
};

class FullyParsedTest : public ParserTest {
   protected:
    ~FullyParsedTest() {
        EXPECT_EQ(parser_->getCurrentToken().getType(), Token::Type::ETX);
    }
};

#endif
