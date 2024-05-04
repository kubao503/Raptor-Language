#include <gtest/gtest.h>

#include "parser_errors.hpp"
#include "parser_test.hpp"

using namespace std::string_literals;

TEST_F(ParserTest, parse_empty_program) {
    SetUp({Token::Type::ETX});

    const auto prog = parser_->parseProgram();
    EXPECT_EQ(prog.statements.size(), 0);
}

TEST_F(ParserTest, parse_if_statement) {
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
}

TEST_F(ParserTest, parse_if_statement_missing_expression) {
    SetUp<Token>({
        {Token::Type::IF_KW, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), SyntaxException);
}

TEST_F(ParserTest, parse_func_def) {
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

TEST_F(ParserTest, parse_func_def_id_ret_type) {
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

TEST_F(ParserTest, parse_func_def_parameter) {
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

TEST_F(ParserTest, parse_func_def_id_parameter) {
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

TEST_F(ParserTest, parse_func_def_two_parameters) {
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

TEST_F(ParserTest, parse_func_def_ref_parameter) {
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

TEST_F(ParserTest, parse_func_def_statements) {
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

TEST_F(ParserTest, parse_void_func_def) {
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

TEST_F(ParserTest, parse_assignment) {
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
    EXPECT_EQ(assignment.lhs, "var");
    ASSERT_TRUE(std::holds_alternative<Constant>(assignment.rhs));

    const auto expression = std::get<Constant>(assignment.rhs);
    EXPECT_EQ(std::get<Integral>(expression.value), 42);
}

TEST_F(ParserTest, parse_assignment_missing_semicolon) {
    SetUp<Token>({
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
    });

    ASSERT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(ParserTest, parse_var_def) {
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

TEST_F(ParserTest, parse_const_var_def) {
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

TEST_F(ParserTest, parse_const_var_def_id_type) {
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
