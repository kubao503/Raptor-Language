#include <gtest/gtest.h>

#include "fixtures.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "parser_errors.hpp"

using namespace std::string_literals;

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

TEST_F(ParserTest, parse_disjuction_expression) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::OR_KW, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<DisjunctionExpression>>(
        varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<DisjunctionExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(lhs));
    const auto& lhsConstant = std::get<Constant>(lhs);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant.value));
    EXPECT_TRUE(std::get<bool>(lhsConstant.value));

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    EXPECT_FALSE(std::get<bool>(rhsConstant.value));
}

TEST_F(ParserTest, parse_nested_disjuction_expressions) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::OR_KW, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::OR_KW, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<DisjunctionExpression>>(
        varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<DisjunctionExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<DisjunctionExpression>>(lhs));
    const auto& lhsExpression = std::get<std::unique_ptr<DisjunctionExpression>>(lhs);

    ASSERT_TRUE(std::holds_alternative<Constant>(lhsExpression->lhs));
    const auto& lhslhsConstant = std::get<Constant>(lhsExpression->lhs);
    EXPECT_TRUE(std::get<bool>(lhslhsConstant.value));

    ASSERT_TRUE(std::holds_alternative<Constant>(lhsExpression->rhs));
    const auto& lhsrhsConstant = std::get<Constant>(lhsExpression->rhs);
    EXPECT_FALSE(std::get<bool>(lhsrhsConstant.value));

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    EXPECT_FALSE(std::get<bool>(rhsConstant.value));
}

TEST_F(ParserTest, parse_conjunction_expression) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::AND_KW, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<ConjunctionExpression>>(
        varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<ConjunctionExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(lhs));
    const auto& lhsConstant = std::get<Constant>(lhs);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant.value));
    EXPECT_TRUE(std::get<bool>(lhsConstant.value));

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    EXPECT_FALSE(std::get<bool>(rhsConstant.value));
}

TEST_F(ParserTest, parse_nested_conjunction_expressions) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::AND_KW, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::AND_KW, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<ConjunctionExpression>>(
        varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<ConjunctionExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<ConjunctionExpression>>(lhs));
    const auto& lhsExpression = std::get<std::unique_ptr<ConjunctionExpression>>(lhs);

    ASSERT_TRUE(std::holds_alternative<Constant>(lhsExpression->lhs));
    const auto& lhslhsConstant = std::get<Constant>(lhsExpression->lhs);
    EXPECT_TRUE(std::get<bool>(lhslhsConstant.value));

    ASSERT_TRUE(std::holds_alternative<Constant>(lhsExpression->rhs));
    const auto& lhsrhsConstant = std::get<Constant>(lhsExpression->rhs);
    EXPECT_FALSE(std::get<bool>(lhsrhsConstant.value));

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    EXPECT_FALSE(std::get<bool>(rhsConstant.value));
}

TEST_F(ParserTest, parse_equal_expression) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::EQ_OP, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<EqualExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<EqualExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(lhs));
    const auto& lhsConstant = std::get<Constant>(lhs);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant.value));
    EXPECT_TRUE(std::get<bool>(lhsConstant.value));

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    EXPECT_FALSE(std::get<bool>(rhsConstant.value));
}

TEST_F(ParserTest, parse_invalid_adjacent_equal_expressions) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::EQ_OP, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::EQ_OP, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::SEMI, {}, {}},
    });

    ASSERT_THROW(parser_->parseProgram(), SyntaxException);
}

TEST_F(ParserTest, parse_not_equal_expression) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::NEQ_OP, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<NotEqualExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<NotEqualExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(lhs));
    const auto& lhsConstant = std::get<Constant>(lhs);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant.value));
    EXPECT_TRUE(std::get<bool>(lhsConstant.value));

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    EXPECT_FALSE(std::get<bool>(rhsConstant.value));
}

TEST_F(ParserTest, parse_rel_expression) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::LT_OP, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<LessThanExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<LessThanExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(lhs));
    const auto& lhsConstant = std::get<Constant>(lhs);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant.value));
    EXPECT_TRUE(std::get<bool>(lhsConstant.value));

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    EXPECT_FALSE(std::get<bool>(rhsConstant.value));
}

TEST_F(ParserTest, parse_additive_expression) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(4), {}},
        {Token::Type::ADD_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(2), {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<AdditionExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<AdditionExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(lhs));
    const auto& lhsConstant = std::get<Constant>(lhs);
    ASSERT_TRUE(std::holds_alternative<Integral>(lhsConstant.value));
    EXPECT_EQ(std::get<Integral>(lhsConstant.value), 4);

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    ASSERT_TRUE(std::holds_alternative<Integral>(rhsConstant.value));
    EXPECT_EQ(std::get<Integral>(rhsConstant.value), 2);
}

TEST_F(ParserTest, parse_multiplicative_expression) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(4), {}},
        {Token::Type::MULT_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(2), {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<MultiplicationExpression>>(
        varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<MultiplicationExpression>>(varDef.expression);

    const auto& lhs = expression->lhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(lhs));
    const auto& lhsConstant = std::get<Constant>(lhs);
    ASSERT_TRUE(std::holds_alternative<Integral>(lhsConstant.value));
    EXPECT_EQ(std::get<Integral>(lhsConstant.value), 4);

    const auto& rhs = expression->rhs;
    ASSERT_TRUE(std::holds_alternative<Constant>(rhs));
    const auto& rhsConstant = std::get<Constant>(rhs);
    ASSERT_TRUE(std::holds_alternative<Integral>(rhsConstant.value));
    EXPECT_EQ(std::get<Integral>(rhsConstant.value), 2);
}

TEST_F(ParserTest, parse_sign_change_expression) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::MIN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(4), {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<SignChangeExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<SignChangeExpression>>(varDef.expression);

    const auto& nestedExp = expression->expr;
    ASSERT_TRUE(std::holds_alternative<Constant>(nestedExp));

    const auto& nestedExpConstant = std::get<Constant>(nestedExp);
    ASSERT_TRUE(std::holds_alternative<Integral>(nestedExpConstant.value));
    EXPECT_EQ(std::get<Integral>(nestedExpConstant.value), 4);
}

TEST_F(ParserTest, parse_negation_expression) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::NOT_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<NegationExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<NegationExpression>>(varDef.expression);

    const auto& nestedExp = expression->expr;
    ASSERT_TRUE(std::holds_alternative<Constant>(nestedExp));

    const auto& nestedExpConstant = std::get<Constant>(nestedExp);
    ASSERT_TRUE(std::holds_alternative<bool>(nestedExpConstant.value));
    EXPECT_TRUE(std::get<bool>(nestedExpConstant.value));
}

TEST_F(ParserTest, parse_type_conversion_expression) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(4), {}},
        {Token::Type::AS_KW, {}, {}},
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<ConversionExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<ConversionExpression>>(varDef.expression);

    const auto& convertedExpr = expression->expr;
    ASSERT_TRUE(std::holds_alternative<Constant>(convertedExpr));
    const auto& constant = std::get<Constant>(convertedExpr);
    ASSERT_TRUE(std::holds_alternative<Integral>(constant.value));
    EXPECT_EQ(std::get<Integral>(constant.value), 4);

    const auto& type = expression->type;
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(type));
    EXPECT_EQ(std::get<BuiltInType>(type), BuiltInType::BOOL);
}

TEST_F(ParserTest, parse_type_conversion_to_id_type) {
    SetUp<Token>({
        {Token::Type::ID, "MyStruct"s, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(4), {}},
        {Token::Type::AS_KW, {}, {}},
        {Token::Type::ID, "MyStruct"s, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<ConversionExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<ConversionExpression>>(varDef.expression);

    const auto& convertedExpr = expression->expr;
    ASSERT_TRUE(std::holds_alternative<Constant>(convertedExpr));
    const auto& constant = std::get<Constant>(convertedExpr);
    ASSERT_TRUE(std::holds_alternative<Integral>(constant.value));
    EXPECT_EQ(std::get<Integral>(constant.value), 4);

    const auto& type = expression->type;
    ASSERT_TRUE(std::holds_alternative<std::string>(type));
    EXPECT_EQ(std::get<std::string>(type), "MyStruct");
}

TEST_F(ParserTest, parse_type_check_expression) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::ID, "checked"s, {}},
        {Token::Type::IS_KW, {}, {}},
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<TypeCheckExpression>>(varDef.expression));
    const auto& expression =
        std::get<std::unique_ptr<TypeCheckExpression>>(varDef.expression);

    const auto& convertedExpr = expression->expr;
    ASSERT_TRUE(std::holds_alternative<Constant>(convertedExpr));
    const auto& constant = std::get<Constant>(convertedExpr);
    ASSERT_TRUE(std::holds_alternative<std::string>(constant.value));
    EXPECT_EQ(std::get<std::string>(constant.value), "checked");

    const auto& type = expression->type;
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(type));
    EXPECT_EQ(std::get<BuiltInType>(type), BuiltInType::BOOL);
}

TEST_F(ParserTest, parse_field_access_expression) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::ID, "myStruct"s, {}},
        {Token::Type::DOT, {}, {}},
        {Token::Type::ID, "firstField"s, {}},
        {Token::Type::DOT, {}, {}},
        {Token::Type::ID, "secondField"s, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<FieldAccessExpression>>(
        varDef.expression));

    const auto& container =
        std::get<std::unique_ptr<FieldAccessExpression>>(varDef.expression);
    EXPECT_EQ(container->field, "secondField");
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<FieldAccessExpression>>(container->expr));

    const auto& nestedContainer =
        std::get<std::unique_ptr<FieldAccessExpression>>(container->expr);
    EXPECT_EQ(nestedContainer->field, "firstField");
}