#include <gtest/gtest.h>

#include "expr_interpreter.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"

class InterpreterTest : public testing::Test {
   protected:
    InterpreterTest()
        : interpreter_{output_} {}

    void SetUp(const std::string& input) {
        stream_ = std::istringstream(input);
        source_ = std::make_unique<Source>(stream_);
        lexer_ = std::make_unique<Lexer>(*source_);
        parser_ = std::make_unique<Parser>(*lexer_);
    }

    std::string interpretAndGetOutput() {
        program_ = parser_->parseProgram();
        interpreter_.interpret(program_);
        return output_.str();
    }

    std::istringstream stream_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
    Program program_;
    Interpreter interpreter_;
    std::stringstream output_;
};

TEST_F(InterpreterTest, empty_program) {
    SetUp("");
    EXPECT_EQ(interpretAndGetOutput(), "");
}

TEST_F(InterpreterTest, basic_print) {
    SetUp("print 5;");
    EXPECT_EQ(interpretAndGetOutput(), "5\n");
}

TEST_F(InterpreterTest, print_new_line) {
    SetUp("print;");
    EXPECT_EQ(interpretAndGetOutput(), "\n");
}

TEST_F(InterpreterTest, global_variable) {
    SetUp(
        "void foo() {"
        "    print x;"
        "}"
        "int x = 5;"
        "foo();");
    EXPECT_EQ(interpretAndGetOutput(), "5\n");
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
    EXPECT_EQ(interpretAndGetOutput(), "24\n");
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
    EXPECT_EQ(interpretAndGetOutput(), "42\n");
}

TEST_F(InterpreterTest, assignment) {
    SetUp(
        "int x = 5;"
        "x = 20;"
        "print x;");
    EXPECT_EQ(interpretAndGetOutput(), "20\n");
}

TEST_F(InterpreterTest, var_def_mismatched_types) {
    SetUp("int x = true;");
    EXPECT_THROW(interpretAndGetOutput(), std::runtime_error);
}

TEST_F(InterpreterTest, assignment_mismatched_types) {
    SetUp(
        "int x = 5;"
        "x = true;");
    EXPECT_THROW(interpretAndGetOutput(), std::runtime_error);
}

TEST_F(InterpreterTest, function_multiple_args) {
    SetUp(
        "void foo(int a, int b) {"
        "    print a;"
        "    print b;"
        "}"
        "foo(5, 7);");
    EXPECT_EQ(interpretAndGetOutput(), "5\n7\n");
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
    EXPECT_EQ(interpretAndGetOutput(), "27\n5\n");
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
    EXPECT_EQ(interpretAndGetOutput(), "27\n27\n");
}

TEST_F(InterpreterTest, function_invalid_arg_count) {
    SetUp(
        "void foo(int a, int b) {"
        "}"
        "foo(5);");
    EXPECT_THROW(interpretAndGetOutput(), std::runtime_error);
}

TEST_F(InterpreterTest, function_invalid_arg_type) {
    SetUp(
        "void foo(str name) {"
        "}"
        "foo(5);");
    EXPECT_THROW(interpretAndGetOutput(), std::runtime_error);
}

TEST_F(InterpreterTest, struct_definition) {
    SetUp(
        "struct Point {"
        "    int x,"
        "    Point y"
        "}");
    interpretAndGetOutput();
    auto structDef = interpreter_.getStructDef("Point");

    EXPECT_EQ(structDef->name, "Point");
    ASSERT_EQ(structDef->fields.size(), 2);

    const auto firstField = structDef->fields.at(0);
    EXPECT_EQ(firstField.name, "x");
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(firstField.type));
    EXPECT_EQ(std::get<BuiltInType>(firstField.type), BuiltInType::INT);

    const auto secondField = structDef->fields.at(1);
    EXPECT_EQ(secondField.name, "y");
    ASSERT_TRUE(std::holds_alternative<std::string>(secondField.type));
    EXPECT_EQ(std::get<std::string>(secondField.type), "Point");
}

TEST_F(InterpreterTest, struct_var_def) {
    SetUp(
        "struct Point {"
        "    int x,"
        "    float y"
        "}"
        "Point p = {1, 2.0};");
    interpretAndGetOutput();

    auto valueRef = interpreter_.readVariable("p");
    ASSERT_TRUE(std::holds_alternative<NamedStructObj>(valueRef->value));
    auto structObj = std::get<NamedStructObj>(valueRef->value);

    ASSERT_EQ(structObj.values.size(), 2);

    const auto firstField = structObj.values.at(0);
    ASSERT_TRUE(std::holds_alternative<Integral>(firstField->value));
    EXPECT_EQ(std::get<Integral>(firstField->value), 1);

    const auto secondField = structObj.values.at(1);
    ASSERT_TRUE(std::holds_alternative<Floating>(secondField->value));
    EXPECT_EQ(std::get<Floating>(secondField->value), 2.0f);
}

TEST_F(InterpreterTest, struct_assignment) {
    SetUp(
        "struct Point {"
        "    int x,"
        "    float y"
        "}"
        "Point p = {1, 2.0};"
        "p = {4, 5.0};");
    interpretAndGetOutput();

    auto valueRef = interpreter_.readVariable("p");
    ASSERT_TRUE(std::holds_alternative<NamedStructObj>(valueRef->value));
    auto structObj = std::get<NamedStructObj>(valueRef->value);

    ASSERT_EQ(structObj.values.size(), 2);

    const auto firstField = structObj.values.at(0);
    ASSERT_TRUE(std::holds_alternative<Integral>(firstField->value));
    EXPECT_EQ(std::get<Integral>(firstField->value), 4);

    const auto secondField = structObj.values.at(1);
    ASSERT_TRUE(std::holds_alternative<Floating>(secondField->value));
    EXPECT_EQ(std::get<Floating>(secondField->value), 5.0f);
}

TEST_F(InterpreterTest, struct_field_access) {
    SetUp(
        "struct MyInteger {"
        "    int x"
        "}"
        "MyInteger i = {1};"
        "print i.x;");
    EXPECT_EQ(interpretAndGetOutput(), "1\n");
}

TEST_F(InterpreterTest, field_access_of_non_struct) {
    SetUp(
        "int x = 5;"
        "print x.field;");
    EXPECT_THROW(interpretAndGetOutput(), std::runtime_error);
}

TEST_F(InterpreterTest, field_access_of_anonymous_struct) {
    SetUp("print {1, 2}.field;");
    EXPECT_THROW(interpretAndGetOutput(), std::runtime_error);
}

TEST_F(InterpreterTest, passing_struct_to_function_by_value) {
    SetUp(
        "struct MyInteger {"
        "    int x"
        "}"
        "void foo(MyInteger i) {"
        "    i = {9};"
        "    print i.x;"
        "}"
        "MyInteger myInteger = {7};"
        "foo(myInteger);"
        "print myInteger.x;");
    EXPECT_EQ(interpretAndGetOutput(), "9\n7\n");
}

TEST_F(InterpreterTest, passing_anonymous_struct_to_function_by_value) {
    SetUp(
        "struct MyInteger {"
        "    int x"
        "}"
        "void foo(MyInteger i) {"
        "    print i.x;"
        "}"
        "foo({7});");
    EXPECT_EQ(interpretAndGetOutput(), "7\n");
}

TEST_F(InterpreterTest, passing_struct_to_function_by_ref) {
    SetUp(
        "struct MyInteger {"
        "    int x"
        "}"
        "void foo(ref MyInteger i) {"
        "    i = {9};"
        "    print i.x;"
        "}"
        "MyInteger myInteger = {7};"
        "foo(ref myInteger);"
        "print myInteger.x;");
    EXPECT_EQ(interpretAndGetOutput(), "9\n9\n");
}
