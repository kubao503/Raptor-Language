#include <gtest/gtest.h>

#include "parser_errors.hpp"
#include "parser_test.hpp"

TEST_F(FullyParsedTest, parse_disjuction_expression) {
    SetUp("bool var = true or false;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto varDef = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE(varDef);

    const auto expression = dynamic_cast<DisjunctionExpression*>(varDef->expression.get());
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
    SetUp("bool var = true or false or false;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<DisjunctionExpression*>(varDef->expression.get());
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
    SetUp("bool var = true and false;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<ConjunctionExpression*>(varDef->expression.get());
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
    SetUp("bool var = true and false and false;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<ConjunctionExpression*>(varDef->expression.get());
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
    SetUp("bool var = true == false;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<EqualExpression*>(varDef->expression.get());
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
    SetUp("bool var = true == false == false;");
    parseAndExpectThrowAt<SyntaxException>({1, 26});
}

TEST_F(FullyParsedTest, parse_not_equal_expression) {
    SetUp("bool var = true != false;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<NotEqualExpression*>(varDef->expression.get());
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
    SetUp("bool var = true < false;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<LessThanExpression*>(varDef->expression.get());
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
    SetUp("int var = 4 + 2;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<AdditionExpression*>(varDef->expression.get());
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
    SetUp("int var = 4 * 2;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression =
        dynamic_cast<MultiplicationExpression*>(varDef->expression.get());
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
    SetUp("int var = -4;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<SignChangeExpression*>(varDef->expression.get());
    ASSERT_TRUE(expression);

    const auto& nestedExp = expression->expr;

    const auto nestedExpConstant = dynamic_cast<Constant*>(nestedExp.get());
    ASSERT_TRUE(nestedExpConstant);

    ASSERT_TRUE(std::holds_alternative<Integral>(nestedExpConstant->value));
    EXPECT_EQ(std::get<Integral>(nestedExpConstant->value), 4);
}

TEST_F(FullyParsedTest, parse_negation_expression) {
    SetUp("bool var = not true;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression =
        dynamic_cast<LogicalNegationExpression*>(varDef->expression.get());
    ASSERT_TRUE(expression);

    const auto& nestedExp = expression->expr;

    const auto nestedExpConstant = dynamic_cast<Constant*>(nestedExp.get());
    ASSERT_TRUE(nestedExpConstant);

    ASSERT_TRUE(std::holds_alternative<bool>(nestedExpConstant->value));
    EXPECT_TRUE(std::get<bool>(nestedExpConstant->value));
}

TEST_F(FullyParsedTest, parse_type_conversion_expression) {
    SetUp("bool var = 4 as bool;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<ConversionExpression*>(varDef->expression.get());
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
    SetUp("MyStruct var = 4 as MyStruct;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<ConversionExpression*>(varDef->expression.get());
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
    SetUp("bool var = checked is bool;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expression = dynamic_cast<TypeCheckExpression*>(varDef->expression.get());
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
    SetUp("int var = myStruct.firstField.secondField;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto lvalue = dynamic_cast<FieldAccessExpression*>(varDef->expression.get());
    ASSERT_TRUE(lvalue);
    EXPECT_EQ(lvalue->field, "secondField");
    const auto nestedlValue = dynamic_cast<FieldAccessExpression*>(lvalue->expr.get());
    ASSERT_TRUE(nestedlValue);
    EXPECT_EQ(nestedlValue->field, "firstField");
}

TEST_F(FullyParsedTest, parse_variable_access) {
    SetUp("bool var = secondVar;");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto varAccess = dynamic_cast<VariableAccess*>(varDef->expression.get());
    ASSERT_TRUE(varAccess);
    EXPECT_EQ(varAccess->name, "secondVar");
}

TEST_F(FullyParsedTest, parse_expression_in_parenthesis) {
    SetUp("bool var = 1 + (1 + 1);");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto expr = dynamic_cast<AdditionExpression*>(varDef->expression.get());
    ASSERT_TRUE(expr);

    EXPECT_TRUE(dynamic_cast<Constant*>(expr->lhs.get()));
    EXPECT_TRUE(dynamic_cast<AdditionExpression*>(expr->rhs.get()));
}

TEST_F(FullyParsedTest, parse_func_call_expression_no_arguments) {
    SetUp("bool var = foo();");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto funcCall = dynamic_cast<FuncCall*>(varDef->expression.get());
    ASSERT_TRUE(funcCall);
    EXPECT_EQ(funcCall->name, "foo");

    const auto& arguments = funcCall->arguments;
    EXPECT_EQ(arguments.size(), 0);
}

TEST_F(FullyParsedTest, parse_func_call_expression_with_arguments) {
    SetUp("bool var = foo(true, false);");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto funcCall = dynamic_cast<FuncCall*>(varDef->expression.get());
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
    SetUp("bool var = foo(ref true);");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto funcCall = dynamic_cast<FuncCall*>(varDef->expression.get());
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
    SetUp("MyStruct var = {};");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto structInitExpr =
        dynamic_cast<StructInitExpression*>(varDef->expression.get());
    ASSERT_TRUE(structInitExpr);

    const auto& structExprs = structInitExpr->exprs;
    ASSERT_TRUE(structExprs.empty());
}

TEST_F(FullyParsedTest, parse_struct_init_expr) {
    SetUp("MyStruct var = {true, false};");

    const auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    const auto  varDef  = dynamic_cast<VarDef*>(prog.statements.at(0).get());
    ASSERT_TRUE( varDef );
    const auto structInitExpr =
        dynamic_cast<StructInitExpression*>(varDef->expression.get());
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
