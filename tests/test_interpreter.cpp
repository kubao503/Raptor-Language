#include <gtest/gtest.h>

#include "expr_interpreter.hpp"
#include "interpreter.hpp"
#include "interpreter_errors.hpp"
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
        program_ = parser_->parseProgram();
    }

    std::string interpretAndGetOutput() {
        interpreter_.interpret(program_);
        return output_.str();
    }

    template <typename Exception>
    void interpretAndExpectThrowAt(Position position) {
        EXPECT_THROW(
            {
                try {
                    interpreter_.interpret(program_);
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
    Program program_;
    Interpreter interpreter_;
    std::stringstream output_;
};

TEST_F(InterpreterTest, empty_program) {
    SetUp("");
    EXPECT_EQ(interpretAndGetOutput(), "");
}

TEST_F(InterpreterTest, print_new_line) {
    SetUp("print;");
    EXPECT_EQ(interpretAndGetOutput(), "\n");
}

TEST_F(InterpreterTest, print_constant) {
    SetUp("print 5;");
    EXPECT_EQ(interpretAndGetOutput(), "5\n");
}

TEST_F(InterpreterTest, var_def) {
    SetUp(
        "int x = 5;"
        "print x;");
    EXPECT_EQ(interpretAndGetOutput(), "5\n");
}

TEST_F(InterpreterTest, var_shadowing) {
    SetUp(
        "int x = 5;"
        "void foo() {"
        "    int x = 22;"
        "    print x;"
        "}"
        "foo();");
    EXPECT_EQ(interpretAndGetOutput(), "22\n");
}

TEST_F(InterpreterTest, var_not_found) {
    SetUp(
        "void foo() {\n"
        "    print x;\n"
        "}\n"
        "foo();");
    interpretAndExpectThrowAt<SymbolNotFound>({2, 11});
}

TEST_F(InterpreterTest, var_redefinition) {
    SetUp(
        "int x = 5;\n"
        "int x = 10;");
    interpretAndExpectThrowAt<VariableRedefinition>({2, 1});
}

TEST_F(InterpreterTest, var_def_mismatched_types) {
    SetUp("int x = true;");
    interpretAndExpectThrowAt<TypeMismatch>({1, 1});
}

TEST_F(InterpreterTest, assignment) {
    SetUp(
        "int x = 5;"
        "x = 20;"
        "print x;");
    EXPECT_EQ(interpretAndGetOutput(), "20\n");
}

TEST_F(InterpreterTest, assignment_mismatched_types) {
    SetUp(
        "int x = 5;\n"
        "x = true;");
    interpretAndExpectThrowAt<TypeMismatch>({2, 1});
}

TEST_F(InterpreterTest, var_def_makes_a_copy) {
    SetUp(
        "int x = 20;"
        "int y = x;"
        "y = 5;"
        "print x;");
    EXPECT_EQ(interpretAndGetOutput(), "20\n");
}

TEST_F(InterpreterTest, assignment_makes_a_copy) {
    SetUp(
        "int x = 20;"
        "int y = 0;"
        "y = x;"
        "y = 5;"
        "print x;");
    EXPECT_EQ(interpretAndGetOutput(), "20\n");
}

TEST_F(InterpreterTest, func_call) {
    SetUp(
        "void fun() {"
        R"(    print "Inside function";)"
        "}"
        "fun();");
    EXPECT_EQ(interpretAndGetOutput(), "Inside function\n");
}

TEST_F(InterpreterTest, function_redefinition) {
    SetUp(
        "void fun() { }\n"
        "int fun(int a) { }");
    interpretAndExpectThrowAt<FunctionRedefinition>({2, 1});
}

TEST_F(InterpreterTest, function_shadowing) {
    SetUp(
        "void fun() { print 1; }"
        "void main() {"
        "    void fun() { print 2; }"
        "    fun();"
        "}"
        "main();");
    EXPECT_EQ(interpretAndGetOutput(), "2\n");
}

TEST_F(InterpreterTest, variable_in_parent_context) {
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

TEST_F(InterpreterTest, function_in_parent_context) {
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

TEST_F(InterpreterTest, function_call_invalid_arg_count) {
    SetUp(
        "void foo(int a, int b) {"
        "}"
        "foo(5);");
    EXPECT_THROW(interpretAndGetOutput(), std::runtime_error);
}

TEST_F(InterpreterTest, function_call_mismatched_arg_type) {
    SetUp(
        "void foo(str name) {\n"
        "}\n"
        "foo(5);");
    interpretAndExpectThrowAt<TypeMismatch>({3, 5});
}

TEST_F(InterpreterTest, function_call_inside_a_function) {
    SetUp(
        "void foo(int a) { print a; }"
        "void bar(int b) {"
        "    foo(b);"
        "}"
        "bar(2);");
    EXPECT_EQ(interpretAndGetOutput(), "2\n");
}

TEST_F(InterpreterTest, function_not_found) {
    SetUp("foo(5);");
    interpretAndExpectThrowAt<SymbolNotFound>({1, 1});
}

TEST_F(InterpreterTest, struct_definition) {
    SetUp(
        "struct Point {"
        "    int x,"
        "    Point y"
        "}");
    interpretAndGetOutput();
    const StructDef* structDef = interpreter_.getStructDef("Point");

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

    auto valueRef = interpreter_.getVariable("p").value();
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

    auto valueRef = interpreter_.getVariable("p").value();
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

TEST_F(InterpreterTest, struct_not_found) {
    SetUp("MyStruct x = {1, 2};");
    interpretAndExpectThrowAt<SymbolNotFound>({1, 1});
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

TEST_F(InterpreterTest, struct_var_invalid_field_type) {
    SetUp(
        "struct MyInteger {\n"
        "    int x\n"
        "}\n"
        "MyInteger i = {true};");
    interpretAndExpectThrowAt<TypeMismatch>({4, 1});
}

TEST_F(InterpreterTest, struct_var_invalid_field_count) {
    SetUp(
        "struct MyInteger {\n"
        "    int x\n"
        "}\n"
        "MyInteger i = {5, 4};");
    interpretAndExpectThrowAt<InvalidFieldCount>({4, 1});
}

TEST_F(InterpreterTest, struct_assignment_invalid_field_count) {
    SetUp(
        "struct MyInteger {\n"
        "    int x\n"
        "}\n"
        "MyInteger i = {7};\n"
        "i = {5, 4};");
    interpretAndExpectThrowAt<InvalidFieldCount>({5, 1});
}

TEST_F(InterpreterTest, struct_assignment_invalid_type) {
    SetUp(
        "struct MyInteger {\n"
        "    int x\n"
        "}\n"
        "MyInteger i = {5};\n"
        "i = {true};");
    interpretAndExpectThrowAt<TypeMismatch>({5, 1});
}

TEST_F(InterpreterTest, field_access_of_anonymous_struct) {
    SetUp("print ({1, 2}).field;");
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

TEST_F(InterpreterTest, passing_struct_to_function_mismatched_field_type) {
    SetUp(
        "struct MyInteger { int x }\n"
        "void foo(MyInteger i) { }\n"
        "foo({true});");
    interpretAndExpectThrowAt<TypeMismatch>({3, 5});
}

TEST_F(InterpreterTest, passing_struct_to_function_invalid_field_count) {
    SetUp(
        "struct MyInteger { int x }\n"
        "void foo(MyInteger i) { }\n"
        "foo({2, 4});");
    interpretAndExpectThrowAt<InvalidFieldCount>({3, 5});
}

TEST_F(InterpreterTest, nested_struct_initialization) {
    SetUp(
        "struct A { int num }"
        "struct B { A a }"
        "B b = {{5}};");
    interpretAndGetOutput();

    auto valueRefB = interpreter_.getVariable("b");
    ASSERT_TRUE(valueRefB);

    ASSERT_TRUE(std::holds_alternative<NamedStructObj>((*valueRefB)->value));
    auto structB = std::get<NamedStructObj>((*valueRefB)->value);
    EXPECT_EQ(structB.structDef->name, "B");
    ASSERT_EQ(structB.values.size(), 1);

    auto valueRefA = structB.values.at(0);
    ASSERT_TRUE(std::holds_alternative<NamedStructObj>(valueRefA->value));
    auto structA = std::get<NamedStructObj>(valueRefA->value);
    EXPECT_EQ(structA.structDef->name, "A");
    ASSERT_EQ(structA.values.size(), 1);

    ASSERT_TRUE(std::holds_alternative<Integral>(structA.values.at(0)->value));
    EXPECT_EQ(std::get<Integral>(structA.values.at(0)->value), 5);
}

TEST_F(InterpreterTest, nested_struct_assignment) {
    SetUp(
        "struct A { int num }"
        "struct B { A a }"
        "B b = {{5}};"
        "b = {{7}};");
    interpretAndGetOutput();

    auto valueRefB = interpreter_.getVariable("b");
    ASSERT_TRUE(valueRefB);

    ASSERT_TRUE(std::holds_alternative<NamedStructObj>((*valueRefB)->value));
    auto structB = std::get<NamedStructObj>((*valueRefB)->value);
    EXPECT_EQ(structB.structDef->name, "B");
    ASSERT_EQ(structB.values.size(), 1);

    auto valueRefA = structB.values.at(0);
    ASSERT_TRUE(std::holds_alternative<NamedStructObj>(valueRefA->value));
    auto structA = std::get<NamedStructObj>(valueRefA->value);
    EXPECT_EQ(structA.structDef->name, "A");
    ASSERT_EQ(structA.values.size(), 1);

    ASSERT_TRUE(std::holds_alternative<Integral>(structA.values.at(0)->value));
    EXPECT_EQ(std::get<Integral>(structA.values.at(0)->value), 7);
}

TEST_F(InterpreterTest, struct_redefinition) {
    SetUp(
        "struct A { int num }\n"
        "struct A { bool truth }");
    interpretAndExpectThrowAt<StructRedefinition>({2, 1});
}

TEST_F(InterpreterTest, struct_shadowing) {
    SetUp(
        "struct A { int num }"
        "void foo() {"
        "    struct A { bool truth }"
        "    A a = {true};"
        "    print a;"
        "}"
        "foo();");
    EXPECT_EQ(interpretAndGetOutput(), "{ true }\n");
}

TEST_F(InterpreterTest, variant_definition) {
    SetUp("variant IntOrBool { int, bool }");
    interpretAndGetOutput();

    const VariantDef* variantDef = interpreter_.getVariantDef("IntOrBool");
    ASSERT_TRUE(variantDef);
    EXPECT_EQ(variantDef->name, "IntOrBool");
    EXPECT_EQ(variantDef->types.size(), 2);

    auto firstType = variantDef->types.at(0);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(firstType));
    EXPECT_EQ(std::get<BuiltInType>(firstType), BuiltInType::INT);

    auto secondType = variantDef->types.at(1);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(secondType));
    EXPECT_EQ(std::get<BuiltInType>(secondType), BuiltInType::BOOL);
}

TEST_F(InterpreterTest, variant_var_def) {
    SetUp(
        "variant IntOrBool { int, bool }"
        "IntOrBool i = 5;");
    interpretAndGetOutput();

    auto var = interpreter_.getVariable("i");
    ASSERT_TRUE(var);
    ASSERT_TRUE(std::holds_alternative<VariantObj>((*var)->value));
    auto variantObj = std::get<VariantObj>((*var)->value);

    EXPECT_EQ(variantObj.variantDef->name, "IntOrBool");
    ASSERT_TRUE(std::holds_alternative<Integral>(variantObj.valueRef->value));
    EXPECT_EQ(std::get<Integral>(variantObj.valueRef->value), 5);
}

TEST_F(InterpreterTest, variant_invalid_type) {
    SetUp(
        "variant IntOrBool { int, bool }\n"
        "IntOrBool i = 2.0;");
    interpretAndExpectThrowAt<TypeMismatch>({2, 1});
}

TEST_F(InterpreterTest, assignment_to_variant_invalid_type) {
    SetUp(
        "variant IntOrBool { int, bool }\n"
        "IntOrBool i = 5;\n"
        "i = 2.0;");
    interpretAndExpectThrowAt<TypeMismatch>({3, 1});
}

TEST_F(InterpreterTest, variant_printing) {
    SetUp(
        "variant IntOrBool { int, bool }"
        "IntOrBool i = 5;"
        "print i;");
    EXPECT_EQ(interpretAndGetOutput(), "5\n");
}

TEST_F(InterpreterTest, variant_assignment) {
    SetUp(
        "variant IntOrBool { int, bool }"
        "IntOrBool i = 5;"
        "i = true;"
        "print i;");
    EXPECT_EQ(interpretAndGetOutput(), "true\n");
}

TEST_F(InterpreterTest, variant_holding_struct) {
    SetUp(
        "struct A { int num }"
        "variant V { A, bool }"
        "A a = {5};"
        "V v = a;"
        "print v;");
    EXPECT_EQ(interpretAndGetOutput(), "{ 5 }\n");
}

TEST_F(InterpreterTest, variant_initialization_with_anonymous_struct) {
    SetUp(
        "struct A { int num }\n"
        "variant V { A, str }\n"
        "V v = {9};");
    interpretAndExpectThrowAt<TypeMismatch>({3, 1});
}

TEST_F(InterpreterTest, assignment_of_anonymous_struct_to_variant) {
    SetUp(
        "struct A { int num }\n"
        "variant V { A, bool }\n"
        "V v = true;\n"
        "v = {9};");
    interpretAndExpectThrowAt<TypeMismatch>({4, 1});
}

TEST_F(InterpreterTest, variant_getting_packed_value) {
    SetUp(
        "variant V { int, bool }"
        "V v = true;"
        "bool a = v as bool;");
    interpretAndGetOutput();
}

TEST_F(InterpreterTest, variant_getting_invalid_type) {
    SetUp(
        "variant V { int, bool }\n"
        "V v = true;\n"
        "bool a = v as int;");
    interpretAndExpectThrowAt<InvalidTypeConversion>({3, 12});
}

TEST_F(InterpreterTest, variant_redefinition) {
    SetUp(
        "variant V { int, bool }\n"
        "variant V { float, str }");
    interpretAndExpectThrowAt<VariantRedefinition>({2, 1});
}

TEST_F(InterpreterTest, variant_shadowing) {
    SetUp(
        "variant V { int, bool }"
        "void foo() {"
        "    variant V { float, str }"
        "    V v = 2.0;"
        "    print v;"
        "}"
        "foo();");
    EXPECT_EQ(interpretAndGetOutput(), "2\n");
}

class TypeConversionTest : public InterpreterTest,
                           public testing::WithParamInterface<
                               std::tuple<std::string, std::string, std::string>> {};

TEST_P(TypeConversionTest, built_in_type_conversions) {
    auto& [value, type, expected] = GetParam();

    SetUp("print " + value + " as " + type + ";");
    EXPECT_EQ(interpretAndGetOutput(), expected + '\n');
}

auto conversionTuples = testing::Values(
    std::make_tuple("5", "float", "5"), std::make_tuple("7.8", "int", "7"),
    std::make_tuple("0", "bool", "false"), std::make_tuple("1", "bool", "true"),
    std::make_tuple("5", "bool", "true"), std::make_tuple("0.4", "bool", "true"),
    std::make_tuple("0.0", "bool", "false"), std::make_tuple("true", "int", "1"),
    std::make_tuple("true", "float", "1"), std::make_tuple("false", "int", "0"),
    std::make_tuple("false", "float", "0"), std::make_tuple(R"("text")", "str", "text"));

INSTANTIATE_TEST_SUITE_P(TypeConversions, TypeConversionTest, conversionTuples);

TEST_F(InterpreterTest, same_struct_conversion) {
    SetUp(
        "struct A { int num }"
        "A a = { 5 };"
        "A b = a as A;"
        "print b;");
    EXPECT_EQ(interpretAndGetOutput(), "{ 5 }\n");
}

TEST_F(InterpreterTest, redefining_struct_with_variant) {
    SetUp(
        "struct A { int num }\n"
        "variant A { int, bool }");
    interpretAndExpectThrowAt<StructRedefinition>({2, 1});
}

TEST_F(InterpreterTest, redefining_variant_with_struct) {
    SetUp(
        "variant A { int, bool }\n"
        "struct A { int num }");
    interpretAndExpectThrowAt<VariantRedefinition>({2, 1});
}

TEST_F(InterpreterTest, return_statement) {
    SetUp(
        "void foo() {"
        "    print 5;"
        "    return;"
        "    print 10;"
        "}"
        "foo();");
    EXPECT_EQ(interpretAndGetOutput(), "5\n");
}

TEST_F(InterpreterTest, return_statement_two_calls) {
    SetUp(
        "void foo() {"
        "    print 2;"
        "    print 5;"
        "    return;"
        "    print 10;"
        "}"
        "foo();"
        "foo();");
    EXPECT_EQ(interpretAndGetOutput(), "2\n5\n2\n5\n");
}

TEST_F(InterpreterTest, return_statement_nested_call) {
    SetUp(
        "void foo() { return; }"
        "void bar() {"
        "    foo();"
        "    print 6;"
        "}"
        "bar();");
    EXPECT_EQ(interpretAndGetOutput(), "6\n");
}

TEST_F(InterpreterTest, returning_value_in_void_func) {
    SetUp(
        "void foo() {\n"
        "    return 5;\n"
        "}\n"
        "foo();");
    interpretAndExpectThrowAt<ReturnTypeMismatch>({1, 1});
}

TEST_F(InterpreterTest, function_call_in_expression) {
    SetUp(
        "int return_one() { return 1; }"
        "int x = return_one();"
        "print x;");
    EXPECT_EQ(interpretAndGetOutput(), "1\n");
}

TEST_F(InterpreterTest, missing_return_statement) {
    SetUp(
        "int foo() { }"
        "foo();");
    interpretAndExpectThrowAt<ReturnTypeMismatch>({1, 1});
}

TEST_F(InterpreterTest, returning_struct) {
    SetUp(
        "struct A { int num }"
        "A foo() {"
        "    A a = {5};"
        "    return a;"
        "}"
        "print foo();");
    EXPECT_EQ(interpretAndGetOutput(), "{ 5 }\n");
}

TEST_F(InterpreterTest, returning_anonymous_struct) {
    SetUp(
        "struct A { int num }"
        "A foo() { return {5}; }"
        "print foo();");
    EXPECT_EQ(interpretAndGetOutput(), "{ 5 }\n");
}

TEST_F(InterpreterTest, returning_wrong_struct) {
    SetUp(
        "struct A { int x, int y }\n"
        "A foo() { return {5}; }\n"
        "foo();");
    interpretAndExpectThrowAt<InvalidFieldCount>({2, 1});
}

TEST_F(InterpreterTest, returning_variant) {
    SetUp(
        "variant V { int, bool }"
        "V foo() { return 5; }"
        "print foo();");
    EXPECT_EQ(interpretAndGetOutput(), "5\n");
}

TEST_F(InterpreterTest, returning_wrong_variant) {
    SetUp(
        "variant V { int, float }\n"
        "V foo() { return true; }\n"
        "print foo();");
    interpretAndExpectThrowAt<ReturnTypeMismatch>({2, 1});
}

TEST_F(InterpreterTest, return_makes_a_copy) {
    SetUp(
        "int x = 5;"
        "int foo(ref int i) { return i; }"
        "int y = foo(ref x);"
        "y = 9;"
        "print x;"
        "print y;");
    EXPECT_EQ(interpretAndGetOutput(), "5\n9\n");
}
