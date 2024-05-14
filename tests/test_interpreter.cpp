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

    std::string interpret() {
        Interpreter(parser_->parseProgram(), output_);
        return output_.str();
    }

    std::istringstream stream_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
    std::stringstream output_;
};

TEST_F(InterpreterTest, empty_program) {
    SetUp("");
    EXPECT_EQ(interpret(), "");
}

TEST_F(InterpreterTest, basic_print) {
    SetUp("print 5;");
    EXPECT_EQ(interpret(), "5\n");
}

TEST_F(InterpreterTest, print_new_line) {
    SetUp("print;");
    EXPECT_EQ(interpret(), "\n");
}

TEST_F(InterpreterTest, global_variable) {
    SetUp(
        "void foo() {"
        "    print x;"
        "}"
        "int x = 5;"
        "foo();");
    EXPECT_EQ(interpret(), "5\n");
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
    EXPECT_EQ(interpret(), "24\n");
}

TEST_F(InterpreterTest, function_in_parent) {
    SetUp(
        "void parent() {"
        "    void nested() {"
        "        second_nested();"
        "    }"
        "    void second_nested() {"
        "        print 42;"
        "    }"
        "    nested();"
        "}"
        "parent();");
    EXPECT_EQ(interpret(), "42\n");
}

TEST_F(InterpreterTest, assignment) {
    SetUp(
        "int x = 5;"
        "x = 20;"
        "print x;");
    EXPECT_EQ(interpret(), "20\n");
}

TEST_F(InterpreterTest, function_multiple_args) {
    SetUp(
        "void foo(int a, int b) {"
        "    print a;"
        "    print b;"
        "}"
        "foo(5, 7);");
    EXPECT_EQ(interpret(), "5\n7\n");
}

TEST_F(InterpreterTest, function_pass_by_value) {
    SetUp(
        "void foo(int a) {"
        "    a = 27;"
        "    print a;"
        "}"
        "int x = 5;"
        "foo(x);"
        "print x;");
    EXPECT_EQ(interpret(), "27\n5\n");
}

TEST_F(InterpreterTest, function_pass_by_ref) {
    SetUp(
        "void foo(ref int a) {"
        "    a = 27;"
        "    print a;"
        "}"
        "int x = 5;"
        "foo(ref x);"
        "print x;");
    EXPECT_EQ(interpret(), "27\n27\n");
}
