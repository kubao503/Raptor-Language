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
    const auto expression = dynamic_cast<DisjunctionExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;

    const auto lhsConstant = dynamic_cast<Constant*>(lhs.get());
    ASSERT_TRUE(lhsConstant);

    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant->value));
    EXPECT_TRUE(std::get<bool>(lhsConstant->value));

    const auto& rhs = expression->rhs;

    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);

    EXPECT_FALSE(std::get<bool>(rhsConstant->value));
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
    const auto expression = dynamic_cast<DisjunctionExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;
    const auto lhsExpression = dynamic_cast<DisjunctionExpression*>(lhs.get());
    ASSERT_TRUE(lhsExpression);

    const auto lhslhsConstant = dynamic_cast<Constant*>(lhsExpression->lhs.get());
    ASSERT_TRUE(lhslhsConstant);
    EXPECT_TRUE(std::get<bool>(lhslhsConstant->value));

    const auto lhsrhsConstant = dynamic_cast<Constant*>(lhsExpression->rhs.get());
    ASSERT_TRUE(lhsrhsConstant);
    EXPECT_FALSE(std::get<bool>(lhsrhsConstant->value));

    const auto& rhs = expression->rhs;
    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);
    EXPECT_FALSE(std::get<bool>(rhsConstant->value));
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
    const auto expression = dynamic_cast<ConjunctionExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;
    const auto lhsConstant = dynamic_cast<Constant*>(lhs.get());
    ASSERT_TRUE(lhsConstant);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant->value));
    EXPECT_TRUE(std::get<bool>(lhsConstant->value));

    const auto& rhs = expression->rhs;
    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);
    EXPECT_FALSE(std::get<bool>(rhsConstant->value));
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
    const auto expression = dynamic_cast<ConjunctionExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;
    const auto lhsExpression = dynamic_cast<ConjunctionExpression*>(lhs.get());
    ASSERT_TRUE(lhsExpression);

    const auto lhslhsConstant = dynamic_cast<Constant*>(lhsExpression->lhs.get());
    ASSERT_TRUE(lhslhsConstant);
    EXPECT_TRUE(std::get<bool>(lhslhsConstant->value));

    const auto lhsrhsConstant = dynamic_cast<Constant*>(lhsExpression->rhs.get());
    ASSERT_TRUE(lhsrhsConstant);
    EXPECT_FALSE(std::get<bool>(lhsrhsConstant->value));

    const auto& rhs = expression->rhs;
    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);
    EXPECT_FALSE(std::get<bool>(rhsConstant->value));
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
    const auto expression = dynamic_cast<EqualExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;
    const auto lhsConstant = dynamic_cast<Constant*>(lhs.get());
    ASSERT_TRUE(lhsConstant);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant->value));
    EXPECT_TRUE(std::get<bool>(lhsConstant->value));

    const auto& rhs = expression->rhs;
    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);
    EXPECT_FALSE(std::get<bool>(rhsConstant->value));
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
    const auto expression = dynamic_cast<NotEqualExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;
    const auto lhsConstant = dynamic_cast<Constant*>(lhs.get());
    ASSERT_TRUE(lhsConstant);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant->value));
    EXPECT_TRUE(std::get<bool>(lhsConstant->value));

    const auto& rhs = expression->rhs;
    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);
    EXPECT_FALSE(std::get<bool>(rhsConstant->value));
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
    const auto expression = dynamic_cast<LessThanExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;
    const auto lhsConstant = dynamic_cast<Constant*>(lhs.get());
    ASSERT_TRUE(lhsConstant);
    ASSERT_TRUE(std::holds_alternative<bool>(lhsConstant->value));
    EXPECT_TRUE(std::get<bool>(lhsConstant->value));

    const auto& rhs = expression->rhs;
    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);
    EXPECT_FALSE(std::get<bool>(rhsConstant->value));
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
    const auto expression = dynamic_cast<AdditionExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;
    const auto lhsConstant = dynamic_cast<Constant*>(lhs.get());
    ASSERT_TRUE(lhsConstant);
    ASSERT_TRUE(std::holds_alternative<Integral>(lhsConstant->value));
    EXPECT_EQ(std::get<Integral>(lhsConstant->value), 4);

    const auto& rhs = expression->rhs;
    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);
    ASSERT_TRUE(std::holds_alternative<Integral>(rhsConstant->value));
    EXPECT_EQ(std::get<Integral>(rhsConstant->value), 2);
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
    const auto expression =
        dynamic_cast<MultiplicationExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& lhs = expression->lhs;
    const auto lhsConstant = dynamic_cast<Constant*>(lhs.get());
    ASSERT_TRUE(lhsConstant);
    ASSERT_TRUE(std::holds_alternative<Integral>(lhsConstant->value));
    EXPECT_EQ(std::get<Integral>(lhsConstant->value), 4);

    const auto& rhs = expression->rhs;
    const auto rhsConstant = dynamic_cast<Constant*>(rhs.get());
    ASSERT_TRUE(rhsConstant);
    ASSERT_TRUE(std::holds_alternative<Integral>(rhsConstant->value));
    EXPECT_EQ(std::get<Integral>(rhsConstant->value), 2);
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
    const auto expression = dynamic_cast<SignChangeExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& nestedExp = expression->expr;

    const auto nestedExpConstant = dynamic_cast<Constant*>(nestedExp.get());
    ASSERT_TRUE(nestedExpConstant);

    ASSERT_TRUE(std::holds_alternative<Integral>(nestedExpConstant->value));
    EXPECT_EQ(std::get<Integral>(nestedExpConstant->value), 4);
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
    const auto expression =
        dynamic_cast<LogicalNegationExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& nestedExp = expression->expr;

    const auto nestedExpConstant = dynamic_cast<Constant*>(nestedExp.get());
    ASSERT_TRUE(nestedExpConstant);

    ASSERT_TRUE(std::holds_alternative<bool>(nestedExpConstant->value));
    EXPECT_TRUE(std::get<bool>(nestedExpConstant->value));
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
    const auto expression = dynamic_cast<ConversionExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& convertedExpr = expression->expr;
    const auto constant = dynamic_cast<Constant*>(convertedExpr.get());
    ASSERT_TRUE(constant);
    ASSERT_TRUE(std::holds_alternative<Integral>(constant->value));
    EXPECT_EQ(std::get<Integral>(constant->value), 4);

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
    const auto expression = dynamic_cast<ConversionExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& convertedExpr = expression->expr;
    const auto constant = dynamic_cast<Constant*>(convertedExpr.get());
    ASSERT_TRUE(constant);
    ASSERT_TRUE(std::holds_alternative<Integral>(constant->value));
    EXPECT_EQ(std::get<Integral>(constant->value), 4);

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
    const auto expression = dynamic_cast<TypeCheckExpression*>(varDef.expression.get());
    ASSERT_TRUE(expression);

    const auto& convertedExpr = expression->expr;
    const auto checkedVar = dynamic_cast<VariableAccess*>(convertedExpr.get());
    ASSERT_TRUE(checkedVar);
    EXPECT_EQ(checkedVar->name, "checked");

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
    const auto lvalue = dynamic_cast<FieldAccessExpression*>(varDef.expression.get());
    ASSERT_TRUE(lvalue);
    EXPECT_EQ(lvalue->field, "secondField");
    const auto nestedlValue = dynamic_cast<FieldAccessExpression*>(lvalue->expr.get());
    ASSERT_TRUE(nestedlValue);
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
    const auto varAccess = dynamic_cast<VariableAccess*>(varDef.expression.get());
    ASSERT_TRUE(varAccess);
    EXPECT_EQ(varAccess->name, "secondVar");
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
    const auto expr = dynamic_cast<AdditionExpression*>(varDef.expression.get());
    ASSERT_TRUE(expr);

    EXPECT_TRUE(dynamic_cast<Constant*>(expr->lhs.get()));
    EXPECT_TRUE(dynamic_cast<AdditionExpression*>(expr->rhs.get()));
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
    const auto funcCall = dynamic_cast<FuncCall*>(varDef.expression.get());
    ASSERT_TRUE(funcCall);
    EXPECT_EQ(funcCall->name, "foo");

    const auto& arguments = funcCall->arguments;
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
    const auto funcCall = dynamic_cast<FuncCall*>(varDef.expression.get());
    ASSERT_TRUE(funcCall);
    EXPECT_EQ(funcCall->name, "foo");

    const auto& arguments = funcCall->arguments;
    ASSERT_EQ(arguments.size(), 2);

    const auto firstArg = dynamic_cast<Constant*>(arguments.at(0).value.get());
    ASSERT_TRUE(firstArg);
    ASSERT_TRUE(std::holds_alternative<bool>(firstArg->value));
    EXPECT_TRUE(std::get<bool>(firstArg->value));
    EXPECT_FALSE(arguments.at(0).ref);

    const auto secondArg = dynamic_cast<Constant*>(arguments.at(1).value.get());
    ASSERT_TRUE(secondArg);
    ASSERT_TRUE(std::holds_alternative<bool>(secondArg->value));
    EXPECT_FALSE(std::get<bool>(secondArg->value));
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
    const auto funcCall = dynamic_cast<FuncCall*>(varDef.expression.get());
    ASSERT_TRUE(funcCall);
    EXPECT_EQ(funcCall->name, "foo");

    const auto& arguments = funcCall->arguments;
    ASSERT_EQ(arguments.size(), 1);
    EXPECT_TRUE(arguments.at(0).ref);

    const auto firstArg = dynamic_cast<Constant*>(arguments.at(0).value.get());
    ASSERT_TRUE(firstArg);
    ASSERT_TRUE(std::holds_alternative<bool>(firstArg->value));
    EXPECT_TRUE(std::get<bool>(firstArg->value));
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
    const auto structInitExpr =
        dynamic_cast<StructInitExpression*>(varDef.expression.get());
    ASSERT_TRUE(structInitExpr);

    const auto& structExprs = structInitExpr->exprs;
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
    const auto structInitExpr =
        dynamic_cast<StructInitExpression*>(varDef.expression.get());
    ASSERT_TRUE(structInitExpr);

    const auto& structExprs = structInitExpr->exprs;
    ASSERT_EQ(structExprs.size(), 2);

    const auto firstConstant = dynamic_cast<Constant*>(structExprs.at(0).get());
    ASSERT_TRUE(firstConstant);
    ASSERT_TRUE(std::holds_alternative<bool>(firstConstant->value));
    EXPECT_TRUE(std::get<bool>(firstConstant->value));

    const auto secondConstant = dynamic_cast<Constant*>(structExprs.at(1).get());
    ASSERT_TRUE(secondConstant);
    ASSERT_TRUE(std::holds_alternative<bool>(secondConstant->value));
    EXPECT_FALSE(std::get<bool>(secondConstant->value));
}
