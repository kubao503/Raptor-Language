#include "ExprIRGenerator.hpp"

void ExprIRGenerator::operator()(const StructInitExpression&) const {}
void ExprIRGenerator::operator()(const DisjunctionExpression&) const {}
void ExprIRGenerator::operator()(const ConjunctionExpression&) const {}
void ExprIRGenerator::operator()(const EqualExpression&) const {}
void ExprIRGenerator::operator()(const NotEqualExpression&) const {}
void ExprIRGenerator::operator()(const LessThanExpression&) const {}
void ExprIRGenerator::operator()(const LessThanOrEqualExpression&) const {}
void ExprIRGenerator::operator()(const GreaterThanExpression&) const {}
void ExprIRGenerator::operator()(const GreaterThanOrEqualExpression&) const {}
void ExprIRGenerator::operator()(const AdditionExpression&) const {}
void ExprIRGenerator::operator()(const SubtractionExpression&) const {}
void ExprIRGenerator::operator()(const MultiplicationExpression&) const {}
void ExprIRGenerator::operator()(const DivisionExpression&) const {}
void ExprIRGenerator::operator()(const SignChangeExpression&) const {}
void ExprIRGenerator::operator()(const LogicalNegationExpression&) const {}
void ExprIRGenerator::operator()(const ConversionExpression&) const {}
void ExprIRGenerator::operator()(const TypeCheckExpression&) const {}
void ExprIRGenerator::operator()(const FieldAccessExpression&) const {}
void ExprIRGenerator::operator()(const Constant&) const {}
void ExprIRGenerator::operator()(const FuncCall&) const {}
void ExprIRGenerator::operator()(const VariableAccess&) const {}