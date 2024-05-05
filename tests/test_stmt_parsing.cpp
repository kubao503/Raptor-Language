#include <gtest/gtest.h>

#include "parser_errors.hpp"
#include "parser_test.hpp"

using namespace std::string_literals;

TEST_F(FullyParsedTest, parse_empty_program) {
    SetUp({Token::Type::ETX});

    const auto prog = parser_->parseProgram();
    EXPECT_EQ(prog.statements.size(), 0);
}

TEST_F(FullyParsedTest, parse_if_statement_empty) {
    SetUp<Token>({
        {Token::Type::IF_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<IfStatement>(prog.statements.at(0)));
    const auto& ifStatement = std::get<IfStatement>(prog.statements.at(0));

    const auto& condition = ifStatement.condition;
    ASSERT_TRUE(std::holds_alternative<Constant>(condition));

    ASSERT_TRUE(ifStatement.statements.empty());
}

TEST_F(FullyParsedTest, parse_if_statement_body) {
    SetUp<Token>({
        {Token::Type::IF_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::SEMI, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<IfStatement>(prog.statements.at(0)));
    const auto& ifStatement = std::get<IfStatement>(prog.statements.at(0));

    const auto& condition = ifStatement.condition;
    ASSERT_TRUE(std::holds_alternative<Constant>(condition));

    ASSERT_EQ(ifStatement.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(ifStatement.statements.at(0)));
}

TEST_F(FullyParsedTest, parse_if_statement_missing_condition) {
    SetUp<Token>({
        {Token::Type::IF_KW, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), SyntaxException);
}

TEST_F(FullyParsedTest, parse_while_statement_empty) {
    SetUp<Token>({
        {Token::Type::WHILE_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<WhileStatement>(prog.statements.at(0)));
    const auto& whileStatement = std::get<WhileStatement>(prog.statements.at(0));

    const auto& condition = whileStatement.condition;
    ASSERT_TRUE(std::holds_alternative<Constant>(condition));

    ASSERT_TRUE(whileStatement.statements.empty());
}

TEST_F(FullyParsedTest, parse_while_statement_body) {
    SetUp<Token>({
        {Token::Type::WHILE_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::SEMI, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<WhileStatement>(prog.statements.at(0)));
    const auto& whileStatement = std::get<WhileStatement>(prog.statements.at(0));

    const auto& condition = whileStatement.condition;
    ASSERT_TRUE(std::holds_alternative<Constant>(condition));

    ASSERT_EQ(whileStatement.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(whileStatement.statements.at(0)));
}

TEST_F(FullyParsedTest, parse_print_statement_empty) {
    SetUp<Token>({
        {Token::Type::PRINT_KW, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    ASSERT_TRUE(std::holds_alternative<PrintStatement>(prog.statements.at(0)));
    const auto& printStatement = std::get<PrintStatement>(prog.statements.at(0));

    ASSERT_FALSE(printStatement.expression);
}

TEST_F(FullyParsedTest, parse_print_statement) {
    SetUp<Token>({
        {Token::Type::PRINT_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    ASSERT_TRUE(std::holds_alternative<PrintStatement>(prog.statements.at(0)));
    const auto& printStatement = std::get<PrintStatement>(prog.statements.at(0));

    ASSERT_TRUE(printStatement.expression);
    ASSERT_TRUE(std::holds_alternative<Constant>(*printStatement.expression));
}

TEST_F(FullyParsedTest, parse_return_statement_empty) {
    SetUp<Token>({
        {Token::Type::RETURN_KW, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    ASSERT_TRUE(std::holds_alternative<ReturnStatement>(prog.statements.at(0)));
    const auto& returnStatement = std::get<ReturnStatement>(prog.statements.at(0));

    ASSERT_FALSE(returnStatement.expression);
}

TEST_F(FullyParsedTest, parse_return_statement) {
    SetUp<Token>({
        {Token::Type::RETURN_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);

    ASSERT_TRUE(std::holds_alternative<ReturnStatement>(prog.statements.at(0)));
    const auto& returnStatement = std::get<ReturnStatement>(prog.statements.at(0));

    ASSERT_TRUE(returnStatement.expression);
    ASSERT_TRUE(std::holds_alternative<Constant>(*returnStatement.expression));
}

TEST_F(FullyParsedTest, parse_func_def) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    const auto& funcDef = std::get<FuncDef>(prog.statements.at(0));
    EXPECT_EQ(funcDef.getName(), "foo");
    EXPECT_EQ(std::get<BuiltInType>(funcDef.getReturnType()), BuiltInType::INT);
    EXPECT_EQ(funcDef.getParameters().size(), 0);
}

TEST_F(FullyParsedTest, parse_func_def_id_ret_type) {
    SetUp<Token>({
        {Token::Type::ID, "MyStruct"s, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    const auto& funcDef = std::get<FuncDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::string>(funcDef.getReturnType()));
    EXPECT_EQ(std::get<std::string>(funcDef.getReturnType()), "MyStruct");
}

TEST_F(FullyParsedTest, parse_func_def_parameter) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "num"s, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    const auto& funcDef = std::get<FuncDef>(prog.statements.at(0));
    ASSERT_EQ(funcDef.getParameters().size(), 1);

    const auto& param = funcDef.getParameters().at(0);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(param.type));
    EXPECT_EQ(std::get<BuiltInType>(param.type), BuiltInType::INT);
    EXPECT_EQ(param.name, "num");
    EXPECT_FALSE(param.ref);
}

TEST_F(FullyParsedTest, parse_func_def_id_parameter) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::ID, "MyInt"s, {}},
        {Token::Type::ID, "num"s, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    const auto& funcDef = std::get<FuncDef>(prog.statements.at(0));
    ASSERT_EQ(funcDef.getParameters().size(), 1);

    const auto& param = funcDef.getParameters().at(0);
    ASSERT_TRUE(std::holds_alternative<std::string>(param.type));
    EXPECT_EQ(std::get<std::string>(param.type), "MyInt");
    EXPECT_EQ(param.name, "num");
}

TEST_F(FullyParsedTest, parse_func_def_two_parameters) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "num"s, {}},
        {Token::Type::CMA, {}, {}},
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "truth"s, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    const auto& funcDef = std::get<FuncDef>(prog.statements.at(0));
    ASSERT_EQ(funcDef.getParameters().size(), 2);

    {
        const auto& param = funcDef.getParameters().at(0);
        ASSERT_TRUE(std::holds_alternative<BuiltInType>(param.type));
        EXPECT_EQ(std::get<BuiltInType>(param.type), BuiltInType::INT);
        EXPECT_EQ(param.name, "num");
        EXPECT_FALSE(param.ref);
    }

    const auto& param = funcDef.getParameters().at(1);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(param.type));
    EXPECT_EQ(std::get<BuiltInType>(param.type), BuiltInType::BOOL);
    EXPECT_EQ(param.name, "truth");
    EXPECT_FALSE(param.ref);
}

TEST_F(ParserTest, parse_func_def_no_parameter_after_comma) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "num"s, {}},
        {Token::Type::CMA, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(FullyParsedTest, parse_func_def_ref_parameter) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::REF_KW, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "num"s, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    const auto& funcDef = std::get<FuncDef>(prog.statements.at(0));
    ASSERT_EQ(funcDef.getParameters().size(), 1);

    const auto& param = funcDef.getParameters().at(0);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(param.type));
    EXPECT_EQ(std::get<BuiltInType>(param.type), BuiltInType::INT);
    EXPECT_EQ(param.name, "num");
    EXPECT_TRUE(param.ref);
}

TEST_F(ParserTest, parse_func_def_no_parameter_after_ref) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::REF_KW, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(FullyParsedTest, parse_func_def_statements) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::IF_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    auto& funcDef = std::get<FuncDef>(prog.statements.at(0));
    const auto& statements = funcDef.getStatements();

    ASSERT_EQ(statements.size(), 1);
    const auto& statement = statements.at(0);
    ASSERT_TRUE(std::holds_alternative<IfStatement>(statement));
}

TEST_F(FullyParsedTest, parse_void_func_def) {
    SetUp<Token>({
        {Token::Type::VOID_KW, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    const auto& funcDef = std::get<FuncDef>(prog.statements.at(0));
    EXPECT_TRUE(std::holds_alternative<VoidType>(funcDef.getReturnType()));
}

TEST_F(ParserTest, parse_void_func_def_no_name_after_void_kw) {
    SetUp<Token>({
        {Token::Type::VOID_KW, {}, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(FullyParsedTest, parse_assignment) {
    SetUp<Token>({
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<Assignment>(prog.statements.at(0)));

    const auto& assignment = std::get<Assignment>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::string>(assignment.lhs));
    EXPECT_EQ(std::get<std::string>(assignment.lhs), "var");
    ASSERT_TRUE(std::holds_alternative<Constant>(assignment.rhs));

    const auto expression = std::get<Constant>(assignment.rhs);
    EXPECT_EQ(std::get<Integral>(expression.value), 42);
}

TEST_F(FullyParsedTest, parse_field_assignment) {
    SetUp<Token>({
        {Token::Type::ID, "myStruct"s, {}},
        {Token::Type::DOT, {}, {}},
        {Token::Type::ID, "firstField"s, {}},
        {Token::Type::DOT, {}, {}},
        {Token::Type::ID, "secondField"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<Assignment>(prog.statements.at(0)));
    const auto& assignment = std::get<Assignment>(prog.statements.at(0));

    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<FieldAccess>>(assignment.lhs));
    const auto& fieldAccess = std::get<std::unique_ptr<FieldAccess>>(assignment.lhs);
    EXPECT_EQ(fieldAccess->field, "secondField");

    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<FieldAccess>>(fieldAccess->container));
    const auto& innerFieldAccess =
        std::get<std::unique_ptr<FieldAccess>>(fieldAccess->container);
    EXPECT_EQ(innerFieldAccess->field, "firstField");
    ASSERT_TRUE(std::holds_alternative<std::string>(innerFieldAccess->container));
    ASSERT_EQ(std::get<std::string>(innerFieldAccess->container), "myStruct");

    ASSERT_TRUE(std::holds_alternative<Constant>(assignment.rhs));
    const auto expression = std::get<Constant>(assignment.rhs);
    EXPECT_TRUE(std::get<bool>(expression.value));
}

TEST_F(FullyParsedTest, parse_assignment_missing_semicolon) {
    SetUp<Token>({
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
    });

    ASSERT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(FullyParsedTest, parse_var_def) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    EXPECT_EQ(varDef.name, "var");
    ASSERT_TRUE(std::holds_alternative<Constant>(varDef.expression));

    const auto expression = std::get<Constant>(varDef.expression);
    ASSERT_EQ(std::get<Integral>(expression.value), 42);
}

TEST_F(FullyParsedTest, parse_const_var_def) {
    SetUp<Token>({
        {Token::Type::CONST_KW, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    EXPECT_EQ(varDef.name, "var");
    ASSERT_TRUE(std::holds_alternative<Constant>(varDef.expression));

    const auto& expression = std::get<Constant>(varDef.expression);
    ASSERT_TRUE(std::holds_alternative<Integral>(expression.value));
    EXPECT_EQ(std::get<Integral>(expression.value), 42);
    EXPECT_TRUE(varDef.isConst);
}

TEST_F(FullyParsedTest, parse_const_var_def_id_type) {
    SetUp<Token>({
        {Token::Type::CONST_KW, {}, {}},
        {Token::Type::ID, "MyStruct"s, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::string>(varDef.type));

    EXPECT_EQ(std::get<std::string>(varDef.type), "MyStruct");
}

TEST_F(ParserTest, parse_const_var_def_invalid_type) {
    SetUp<Token>({
        {Token::Type::CONST_KW, {}, {}},
        {Token::Type::UNKNOWN, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
        {Token::Type::SEMI, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(FullyParsedTest, parse_func_call_statement) {
    SetUp<Token>({
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncCall>(prog.statements.at(0)));
    const auto& funcCall = std::get<FuncCall>(prog.statements.at(0));

    EXPECT_EQ(funcCall.name, "foo");
    EXPECT_TRUE(funcCall.arguments.empty());
}

TEST_F(FullyParsedTest, parse_func_call_statement_args) {
    SetUp<Token>({
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::CMA, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncCall>(prog.statements.at(0)));
    const auto& funcCall = std::get<FuncCall>(prog.statements.at(0));

    EXPECT_EQ(funcCall.name, "foo");
    EXPECT_EQ(funcCall.arguments.size(), 2);
}

TEST_F(ParserTest, parse_empty_struct_def) {
    SetUp<Token>({
        {Token::Type::STRUCT_KW, {}, {}},
        {Token::Type::ID, "MyStruct"s, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<StructDef>(prog.statements.at(0)));
    const auto& structDef = std::get<StructDef>(prog.statements.at(0));

    EXPECT_EQ(structDef.name, "MyStruct");
    EXPECT_TRUE(structDef.fields.empty());
}

TEST_F(ParserTest, parse_struct_def) {
    SetUp<Token>({
        {Token::Type::STRUCT_KW, {}, {}},
        {Token::Type::ID, "Point"s, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "x"s, {}},
        {Token::Type::CMA, {}, {}},
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "y"s, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<StructDef>(prog.statements.at(0)));
    const auto& structDef = std::get<StructDef>(prog.statements.at(0));

    EXPECT_EQ(structDef.name, "Point");
    ASSERT_EQ(structDef.fields.size(), 2);

    const auto& firstField = structDef.fields.at(0);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(firstField.type));
    EXPECT_EQ(std::get<BuiltInType>(firstField.type), BuiltInType::INT);
    EXPECT_EQ(firstField.name, "x");

    const auto& secondField = structDef.fields.at(1);
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(secondField.type));
    EXPECT_EQ(std::get<BuiltInType>(secondField.type), BuiltInType::BOOL);
    EXPECT_EQ(secondField.name, "y");
}
