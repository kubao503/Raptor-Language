#include <gtest/gtest.h>

#include "parser_errors.hpp"
#include "parser_test.hpp"

using namespace std::string_literals;

TEST_F(FullyParsedTest, parse_disjuction_expression) {
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

TEST_F(FullyParsedTest, parse_nested_disjuction_expressions) {
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

TEST_F(FullyParsedTest, parse_conjunction_expression) {
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

TEST_F(FullyParsedTest, parse_nested_conjunction_expressions) {
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

TEST_F(FullyParsedTest, parse_equal_expression) {
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

TEST_F(FullyParsedTest, parse_not_equal_expression) {
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

TEST_F(FullyParsedTest, parse_rel_expression) {
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

TEST_F(FullyParsedTest, parse_additive_expression) {
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

TEST_F(FullyParsedTest, parse_multiplicative_expression) {
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

TEST_F(FullyParsedTest, parse_sign_change_expression) {
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

TEST_F(FullyParsedTest, parse_negation_expression) {
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

TEST_F(FullyParsedTest, parse_type_conversion_expression) {
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

TEST_F(FullyParsedTest, parse_type_conversion_to_id_type) {
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

TEST_F(FullyParsedTest, parse_type_check_expression) {
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
    ASSERT_TRUE(std::holds_alternative<VariableAccess>(convertedExpr));
    const auto& checkedVar = std::get<VariableAccess>(convertedExpr);
    EXPECT_EQ(checkedVar.name, "checked");

    const auto& type = expression->type;
    ASSERT_TRUE(std::holds_alternative<BuiltInType>(type));
    EXPECT_EQ(std::get<BuiltInType>(type), BuiltInType::BOOL);
}

TEST_F(FullyParsedTest, parse_field_access_expression) {
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

    const auto& lvalue =
        std::get<std::unique_ptr<FieldAccessExpression>>(varDef.expression);
    EXPECT_EQ(lvalue->field, "secondField");
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<FieldAccessExpression>>(lvalue->expr));

    const auto& nestedlValue =
        std::get<std::unique_ptr<FieldAccessExpression>>(lvalue->expr);
    EXPECT_EQ(nestedlValue->field, "firstField");
}

TEST_F(FullyParsedTest, parse_variable_access) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::ID, "secondVar"s, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<VariableAccess>(varDef.expression));

    const auto& varAccess = std::get<VariableAccess>(varDef.expression);
    EXPECT_EQ(varAccess.name, "secondVar");
}

TEST_F(FullyParsedTest, parse_expression_in_parenthesis) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(1), {}},
        {Token::Type::ADD_OP, {}, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(1), {}},
        {Token::Type::ADD_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(1), {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(
        std::holds_alternative<std::unique_ptr<AdditionExpression>>(varDef.expression));

    const auto& expr = std::get<std::unique_ptr<AdditionExpression>>(varDef.expression);
    EXPECT_TRUE(std::holds_alternative<Constant>(expr->lhs));
    EXPECT_TRUE(std::holds_alternative<std::unique_ptr<AdditionExpression>>(expr->rhs));
}

TEST_F(FullyParsedTest, parse_func_call_expression_no_arguments) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<FuncCall>(varDef.expression));

    const auto& funcCall = std::get<FuncCall>(varDef.expression);
    EXPECT_EQ(funcCall.name, "foo");

    const auto& arguments = funcCall.arguments;
    EXPECT_EQ(arguments.size(), 0);
}

TEST_F(FullyParsedTest, parse_func_call_expression_with_arguments) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
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
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<FuncCall>(varDef.expression));

    const auto& funcCall = std::get<FuncCall>(varDef.expression);
    EXPECT_EQ(funcCall.name, "foo");

    const auto& arguments = funcCall.arguments;
    ASSERT_EQ(arguments.size(), 2);

    ASSERT_TRUE(std::holds_alternative<Constant>(arguments.at(0).value));
    const auto& firstArg = std::get<Constant>(arguments.at(0).value);
    ASSERT_TRUE(std::holds_alternative<bool>(firstArg.value));
    EXPECT_TRUE(std::get<bool>(firstArg.value));
    EXPECT_FALSE(arguments.at(0).ref);

    ASSERT_TRUE(std::holds_alternative<Constant>(arguments.at(1).value));
    const auto& secondArg = std::get<Constant>(arguments.at(1).value);
    ASSERT_TRUE(std::holds_alternative<bool>(secondArg.value));
    EXPECT_FALSE(std::get<bool>(secondArg.value));
    EXPECT_FALSE(arguments.at(1).ref);
}

TEST_F(FullyParsedTest, parse_func_call_expression_with_ref_argument) {
    SetUp<Token>({
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::ID, "foo"s, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::REF_KW, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<FuncCall>(varDef.expression));

    const auto& funcCall = std::get<FuncCall>(varDef.expression);
    EXPECT_EQ(funcCall.name, "foo");

    const auto& arguments = funcCall.arguments;
    ASSERT_EQ(arguments.size(), 1);
    EXPECT_TRUE(arguments.at(0).ref);

    ASSERT_TRUE(std::holds_alternative<Constant>(arguments.at(0).value));
    const auto& firstArg = std::get<Constant>(arguments.at(0).value);
    ASSERT_TRUE(std::holds_alternative<bool>(firstArg.value));
    EXPECT_TRUE(std::get<bool>(firstArg.value));
}

TEST_F(FullyParsedTest, parse_struct_init_expr_empty) {
    SetUp<Token>({
        {Token::Type::ID, "MyStruct"s, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<StructInitExpression>(varDef.expression));
    const auto& structExprs = std::get<StructInitExpression>(varDef.expression).exprs;
    ASSERT_TRUE(structExprs.empty());
}

TEST_F(FullyParsedTest, parse_struct_init_expr) {
    SetUp<Token>({
        {Token::Type::ID, "MyStruct"s, {}},
        {Token::Type::ID, "var"s, {}},
        {Token::Type::ASGN_OP, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::TRUE_CONST, true, {}},
        {Token::Type::CMA, {}, {}},
        {Token::Type::FALSE_CONST, false, {}},
        {Token::Type::R_C_BR, {}, {}},
        {Token::Type::SEMI, {}, {}},
    });

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto& varDef = std::get<VarDef>(prog.statements.at(0));
    ASSERT_TRUE(std::holds_alternative<StructInitExpression>(varDef.expression));
    const auto& structExprs = std::get<StructInitExpression>(varDef.expression).exprs;
    ASSERT_EQ(structExprs.size(), 2);

    ASSERT_TRUE(std::holds_alternative<Constant>(structExprs.at(0)));
    const auto& firstConstant = std::get<Constant>(structExprs.at(0));
    ASSERT_TRUE(std::holds_alternative<bool>(firstConstant.value));
    EXPECT_TRUE(std::get<bool>(firstConstant.value));

    ASSERT_TRUE(std::holds_alternative<Constant>(structExprs.at(1)));
    const auto& secondConstant = std::get<Constant>(structExprs.at(1));
    ASSERT_TRUE(std::holds_alternative<bool>(secondConstant.value));
    EXPECT_FALSE(std::get<bool>(secondConstant.value));
}
