#include <gtest/gtest.h>

#include "parser_errors.hpp"
#include "parser_test.hpp"

TEST_F(FullyParsedTest, parse_empty_program) {
    Init("");

    const auto prog = parser_->parseProgram();
    EXPECT_EQ(prog.statements.size(), 0);
}

TEST_F(ParserTest, unknown_statement) {
    Init("unknown");

    EXPECT_THROW(parser_->parseProgram(), SyntaxException);
}

TEST_F(FullyParsedTest, parse_if_statement_empty) {
    Init(
        "if true {"
        "}");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    const auto ifStatement = dynamic_cast<IfStatement*>(prog.statements.at(0).get());
    ASSERT_TRUE(ifStatement);

    const auto& condition = ifStatement->condition;
    ASSERT_TRUE(dynamic_cast<Constant*>(condition.get()));

    ASSERT_TRUE(ifStatement->statements.empty());
}

TEST_F(FullyParsedTest, parse_if_statement_body) {
    Init(
        "if true {"
        "bool var = true;"
        "}");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    const auto ifStatement = dynamic_cast<IfStatement*>(prog.statements.at(0).get());
    ASSERT_TRUE(ifStatement);

    const auto& condition = ifStatement->condition;
    ASSERT_TRUE(dynamic_cast<Constant*>(condition.get()));

    ASSERT_EQ(ifStatement->statements.size(), 1);
    ASSERT_TRUE(dynamic_cast<VarDef*>(ifStatement->statements.at(0).get()));
}

TEST_F(ParserTest, parse_if_statement_missing_condition) {
    Init(
        "if {\n"
        "}");
    parseAndExpectThrowAt<SyntaxException>({1, 4});
}

TEST_F(FullyParsedTest, parse_while_statement_empty) {
    Init(
        "while true {"
        "}");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    const auto whileStatement =
        dynamic_cast<WhileStatement*>(prog.statements.at(0).get());
    ASSERT_TRUE(whileStatement);

    const auto& condition = whileStatement->condition;
    ASSERT_TRUE(dynamic_cast<Constant*>(condition.get()));

    ASSERT_TRUE(whileStatement->statements.empty());
}

TEST_F(FullyParsedTest, parse_while_statement_body) {
    Init(
        "while true {"
        "    bool var = true;"
        "}");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    const auto whileStatement =
        dynamic_cast<WhileStatement*>(prog.statements.at(0).get());
    ASSERT_TRUE(whileStatement);

    const auto& condition = whileStatement->condition;
    ASSERT_TRUE(dynamic_cast<Constant*>(condition.get()));

    ASSERT_EQ(whileStatement->statements.size(), 1);
    ASSERT_TRUE(dynamic_cast<VarDef*>(whileStatement->statements.at(0).get()));
}

TEST_F(FullyParsedTest, parse_print_statement_empty) {
    Init("print;");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    const auto printStatement =
        dynamic_cast<PrintStatement*>(prog.statements.at(0).get());
    ASSERT_TRUE(printStatement);

    ASSERT_FALSE(printStatement->expression);
}

TEST_F(FullyParsedTest, parse_print_statement) {
    Init("print true;");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    const auto printStatement =
        dynamic_cast<PrintStatement*>(prog.statements.at(0).get());
    ASSERT_TRUE(printStatement);

    ASSERT_TRUE(printStatement->expression);
    ASSERT_TRUE(dynamic_cast<Constant*>(printStatement->expression.get()));
}

TEST_F(FullyParsedTest, parse_return_statement_empty) {
    Init("return;");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    const auto returnStatement =
        dynamic_cast<ReturnStatement*>(prog.statements.at(0).get());
    ASSERT_TRUE(returnStatement);

    ASSERT_FALSE(returnStatement->expression);
}

TEST_F(FullyParsedTest, parse_return_statement) {
    Init("return true;");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    const auto returnStatement =
        dynamic_cast<ReturnStatement*>(prog.statements.at(0).get());
    ASSERT_TRUE(returnStatement);

    ASSERT_TRUE(returnStatement->expression);
    ASSERT_TRUE(dynamic_cast<Constant*>(returnStatement->expression.get()));
}

TEST_F(FullyParsedTest, parse_func_def) {
    Init(
        "int foo() {"
        "}");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    const auto funcDef = dynamic_cast<FuncDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcDef);

    EXPECT_EQ(funcDef->getName(), "foo");
    EXPECT_EQ(std::get<BuiltInType>(funcDef->getReturnType()), BuiltInType::INT);
    EXPECT_EQ(funcDef->getParameters().size(), 0);
}

TEST_F(FullyParsedTest, parse_func_def_id_ret_type) {
    Init(
        "MyStruct foo() {"
        "}");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcDef = dynamic_cast<FuncDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcDef);
    ASSERT_TRUE(std::holds_alternative<std::string>(funcDef->getReturnType()));
    EXPECT_EQ(std::get<std::string>(funcDef->getReturnType()), "MyStruct");
}

TEST_F(FullyParsedTest, parse_func_def_parameter) {
    Init(
        "int foo(int num) {"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcDef = dynamic_cast<FuncDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcDef);
    ASSERT_EQ(funcDef->getParameters().size(), 1);

    const auto& param = funcDef->getParameters().at(0);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(param.type));
    EXPECT_EQ(std::get<BuiltInType>(param.type), BuiltInType::INT);
    EXPECT_EQ(param.name, "num");
    EXPECT_FALSE(param.ref);
}

TEST_F(FullyParsedTest, parse_func_def_id_parameter) {
    Init(
        "int foo(MyInt num) {"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcDef = dynamic_cast<FuncDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcDef);
    ASSERT_EQ(funcDef->getParameters().size(), 1);

    const auto& param = funcDef->getParameters().at(0);
    ASSERT_TRUE(std::holds_alternative<std::string>(param.type));
    EXPECT_EQ(std::get<std::string>(param.type), "MyInt");
    EXPECT_EQ(param.name, "num");
}

TEST_F(FullyParsedTest, parse_func_def_two_parameters) {
    Init(
        "int foo(int num, bool truth) {"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcDef = dynamic_cast<FuncDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcDef);
    ASSERT_EQ(funcDef->getParameters().size(), 2);

    {
        const auto& param = funcDef->getParameters().at(0);
        ASSERT_TRUE(std::holds_alternative<BuiltInType>(param.type));
        EXPECT_EQ(std::get<BuiltInType>(param.type), BuiltInType::INT);
        EXPECT_EQ(param.name, "num");
        EXPECT_FALSE(param.ref);
    }

    const auto& param = funcDef->getParameters().at(1);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(param.type));
    EXPECT_EQ(std::get<BuiltInType>(param.type), BuiltInType::BOOL);
    EXPECT_EQ(param.name, "truth");
    EXPECT_FALSE(param.ref);
}

TEST_F(ParserTest, parse_func_def_no_parameter_after_comma) {
    Init(
        "int foo(int num, ) {\n"
        "}");

    parseAndExpectThrowAt<SyntaxException>({1, 18});
}

TEST_F(FullyParsedTest, parse_func_def_ref_parameter) {
    Init(
        "int foo(ref int num) {"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcDef = dynamic_cast<FuncDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcDef);
    ASSERT_EQ(funcDef->getParameters().size(), 1);

    const auto& param = funcDef->getParameters().at(0);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(param.type));
    EXPECT_EQ(std::get<BuiltInType>(param.type), BuiltInType::INT);
    EXPECT_EQ(param.name, "num");
    EXPECT_TRUE(param.ref);
}

TEST_F(ParserTest, parse_func_def_no_parameter_after_ref) {
    Init(
        "int foo(ref) {\n"
        "}");

    parseAndExpectThrowAt<SyntaxException>({1, 12});
}

TEST_F(FullyParsedTest, parse_func_def_statements) {
    Init(
        "int foo() {"
        "    if true {"
        "    }"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcDef = dynamic_cast<FuncDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcDef);
    const auto& statements = funcDef->getStatements();

    ASSERT_EQ(statements.size(), 1);
    const auto& statement = statements.at(0);
    ASSERT_TRUE(dynamic_cast<IfStatement*>(statement.get()));
}

TEST_F(FullyParsedTest, parse_void_func_def) {
    Init(
        "void foo() {"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcDef = dynamic_cast<FuncDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcDef);
    EXPECT_TRUE(std::holds_alternative<VoidType>(funcDef->getReturnType()));
}

TEST_F(ParserTest, parse_void_func_def_no_name_after_void_kw) {
    Init(
        "void () {\n"
        "}");

    parseAndExpectThrowAt<SyntaxException>({1, 6});
}

TEST_F(FullyParsedTest, parse_assignment) {
    Init("var = 42;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto assignment = dynamic_cast<Assignment*>(prog.statements.at(0).get());
    ASSERT_TRUE(assignment);
    ASSERT_TRUE(std::holds_alternative<std::string>(assignment->lhs));
    EXPECT_EQ(std::get<std::string>(assignment->lhs), "var");

    const auto expression = dynamic_cast<Constant*>(assignment->rhs.get());
    ASSERT_TRUE(expression);

    EXPECT_EQ(std::get<Integral>(expression->value), 42);
}

TEST_F(FullyParsedTest, parse_field_assignment) {
    Init("myStruct.firstField.secondField = true;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto assignment = dynamic_cast<Assignment*>(prog.statements.at(0).get());
    ASSERT_TRUE(assignment);

    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<FieldAccess>>(assignment->lhs));
    const auto& fieldAccess = std::get<std::unique_ptr<FieldAccess>>(assignment->lhs);
    EXPECT_EQ(fieldAccess->field, "secondField");

    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<FieldAccess>>(fieldAccess->container));
    const auto& innerFieldAccess =
        std::get<std::unique_ptr<FieldAccess>>(fieldAccess->container);
    EXPECT_EQ(innerFieldAccess->field, "firstField");

    ASSERT_TRUE(std::holds_alternative<std::string>(innerFieldAccess->container));
    ASSERT_EQ(std::get<std::string>(innerFieldAccess->container), "myStruct");

    const auto expression = dynamic_cast<Constant*>(assignment->rhs.get());
    ASSERT_TRUE(expression);
    EXPECT_TRUE(std::get<bool>(expression->value));
}

TEST_F(FullyParsedTest, parse_assignment_missing_semicolon) {
    Init("var = 42");

    parseAndExpectThrowAt<SyntaxException>({1, 9});
}

TEST_F(FullyParsedTest, parse_var_def) {
    Init("int var = 42;");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    const auto varDef = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(varDef);

    EXPECT_EQ(varDef->name, "var");

    const auto expression = dynamic_cast<Constant*>(varDef->expression.get());
    ASSERT_TRUE(expression);

    ASSERT_EQ(std::get<Integral>(expression->value), 42);
}

TEST_F(FullyParsedTest, parse_const_var_def) {
    Init("const int var = 42;");

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    const auto varDef = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(varDef);

    EXPECT_EQ(varDef->name, "var");

    const auto expression = dynamic_cast<Constant*>(varDef->expression.get());
    ASSERT_TRUE(expression);

    ASSERT_TRUE(std::holds_alternative<Integral>(expression->value));
    EXPECT_EQ(std::get<Integral>(expression->value), 42);
    EXPECT_TRUE(varDef->isConst);
}

TEST_F(FullyParsedTest, parse_const_var_def_id_type) {
    Init("const MyStruct var = 42;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto varDef = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(varDef);

    ASSERT_TRUE(std::holds_alternative<std::string>(varDef->type));
    EXPECT_EQ(std::get<std::string>(varDef->type), "MyStruct");
}

TEST_F(ParserTest, parse_const_var_def_invalid_type) {
    Init("const if var = 42;");

    parseAndExpectThrowAt<SyntaxException>({1, 7});
}

TEST_F(FullyParsedTest, parse_func_call_statement) {
    Init("foo();");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcCall = dynamic_cast<FuncCall*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcCall);

    EXPECT_EQ(funcCall->name, "foo");
    EXPECT_TRUE(funcCall->arguments.empty());
}

TEST_F(FullyParsedTest, parse_func_call_statement_args) {
    Init("foo(true, false);");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto funcCall = dynamic_cast<FuncCall*>(prog.statements.at(0).get());
    ASSERT_TRUE(funcCall);

    EXPECT_EQ(funcCall->name, "foo");
    EXPECT_EQ(funcCall->arguments.size(), 2);
}

TEST_F(FullyParsedTest, parse_empty_struct_def) {
    Init(
        "struct MyStruct {"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto structDef = dynamic_cast<StructDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(structDef);

    EXPECT_EQ(structDef->name, "MyStruct");
    EXPECT_TRUE(structDef->fields.empty());
}

TEST_F(FullyParsedTest, parse_struct_def) {
    Init(
        "struct Point {"
        "    int x,"
        "    bool y"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto structDef = dynamic_cast<StructDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(structDef);

    EXPECT_EQ(structDef->name, "Point");
    ASSERT_EQ(structDef->fields.size(), 2);

    const auto& firstField = structDef->fields.at(0);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(firstField.type));
    EXPECT_EQ(std::get<BuiltInType>(firstField.type), BuiltInType::INT);
    EXPECT_EQ(firstField.name, "x");

    const auto& secondField = structDef->fields.at(1);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(secondField.type));
    EXPECT_EQ(std::get<BuiltInType>(secondField.type), BuiltInType::BOOL);
    EXPECT_EQ(secondField.name, "y");
}

TEST_F(FullyParsedTest, parse_variant_def) {
    Init(
        "variant IntOrBool {"
        "    int,"
        "    bool"
        "}");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto variantDef = dynamic_cast<VariantDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(variantDef);

    EXPECT_EQ(variantDef->name, "IntOrBool");
    ASSERT_EQ(variantDef->types.size(), 2);

    const auto& firstType = variantDef->types.at(0);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(firstType));
    EXPECT_EQ(std::get<BuiltInType>(firstType), BuiltInType::INT);

    const auto& secondType = variantDef->types.at(1);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(secondType));
    EXPECT_EQ(std::get<BuiltInType>(secondType), BuiltInType::BOOL);
}

TEST_F(ParserTest, parse_invalid_variant_with_no_types) {
    Init(
        "variant IntOrBool {\n"
        "}");

    parseAndExpectThrowAt<NoTypesInVariant>({2, 1});
}
