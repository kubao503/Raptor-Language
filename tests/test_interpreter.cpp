#include <gtest/gtest.h>

#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"

class InterpreterTest : public testing::Test {
   protected:
    void SetUp(const std::string& input) {
        stream_ = std::istringstream(input);
        source_ = std::make_unique<Source>(stream_);
        lexer_ = std::make_unique<Lexer>(*source_);
        parser_ = std::make_unique<Parser>(*lexer_);
    }

    void parse() { Interpreter(parser_->parseProgram(), output_); }

    std::istringstream stream_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
    std::stringstream output_;
};

TEST_F(InterpreterTest, empty_program) {
    SetUp("");
    parse();
    EXPECT_EQ(output_.str(), "");
}

TEST_F(InterpreterTest, basic_print) {
    SetUp("print 5;");
    parse();
    EXPECT_EQ(output_.str(), "5\n");
}

TEST_F(InterpreterTest, global_variable) {
    SetUp(
        "void foo() {"
        "    print x;"
        "}"
        "int x = 5;"
        "foo();");
    parse();
    EXPECT_EQ(output_.str(), "5\n");
}

TEST_F(InterpreterTest, variable_in_parent) {
    SetUp(
        "void parent() {"
        "    void nested() {"
        "        print x;"
        "    }"
        "    int x = 24;"
        "    nested();"
        "}"
        "parent();");
    parse();
    EXPECT_EQ(output_.str(), "24\n");
}
