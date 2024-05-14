#include "expr_interpreter.hpp"

#include "interpreter.hpp"

ExpressionInterpreter::ExpressionInterpreter(const Interpreter& interpreter)
    : interpreter_{interpreter} {}

void ExpressionInterpreter::operator()(const StructInitExpression&) const {}
void ExpressionInterpreter::operator()(const DisjunctionExpression&) const {}
void ExpressionInterpreter::operator()(const ConjunctionExpression&) const {}
void ExpressionInterpreter::operator()(const EqualExpression&) const {}
void ExpressionInterpreter::operator()(const NotEqualExpression&) const {}
void ExpressionInterpreter::operator()(const LessThanExpression&) const {}
void ExpressionInterpreter::operator()(const LessThanOrEqualExpression&) const {}
void ExpressionInterpreter::operator()(const GreaterThanExpression&) const {}
void ExpressionInterpreter::operator()(const GreaterThanOrEqualExpression&) const {}
void ExpressionInterpreter::operator()(const AdditionExpression&) const {}
void ExpressionInterpreter::operator()(const SubtractionExpression&) const {}
void ExpressionInterpreter::operator()(const MultiplicationExpression&) const {}
void ExpressionInterpreter::operator()(const DivisionExpression&) const {}
void ExpressionInterpreter::operator()(const SignChangeExpression&) const {}
void ExpressionInterpreter::operator()(const LogicalNegationExpression&) const {}
void ExpressionInterpreter::operator()(const ConversionExpression&) const {}
void ExpressionInterpreter::operator()(const TypeCheckExpression&) const {}
void ExpressionInterpreter::operator()(const FieldAccessExpression&) const {}

void ExpressionInterpreter::operator()(const Constant& expr) const {
    lastResult_ = expr.value;
}

void ExpressionInterpreter::operator()(const FuncCall&) const {}

void ExpressionInterpreter::operator()(const VariableAccess& expr) const {
    lastResult_ = interpreter_.readVariable(expr.name)->value;
}
