#include "printer.hpp"

#include <iostream>

#include "magic_enum/magic_enum.hpp"

std::ostream& operator<<(std::ostream& stream, const Program& program) {
    for (const auto& stmt : program.statements) {
        auto printer = StatementPrinter();
        stmt->accept(printer);
    }
    return stream;
}

ExpressionPrinter BasePrinter::getSubExprPrinter() const {
    return ExpressionPrinter(indent_ + indentWidth_);
}

StatementPrinter BasePrinter::getSubStmtPrinter() const {
    return StatementPrinter(indent_ + indentWidth_);
}

void ExpressionPrinter::printBinaryExpression(const auto& expression) const {
    expression.lhs->accept(getSubExprPrinter());
    expression.rhs->accept(getSubExprPrinter());
}

void ExpressionPrinter::operator()(const StructInitExpression& expr) const {
    std::cout << getPrefix() << "StructInitExpression\n";
    for (const auto& expr : expr.exprs)
        expr->accept(getSubExprPrinter());
}

void ExpressionPrinter::operator()(const DisjunctionExpression& disjunction) const {
    std::cout << getPrefix() << "DisjunctionExpression\n";
    printBinaryExpression(disjunction);
}

void ExpressionPrinter::operator()(const ConjunctionExpression& conjunction) const {
    std::cout << getPrefix() << "ConjunctionExpression\n";
    printBinaryExpression(conjunction);
}

void ExpressionPrinter::operator()(const EqualExpression& expr) const {
    std::cout << getPrefix() << "EqualExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const NotEqualExpression& expr) const {
    std::cout << getPrefix() << "NotEqualExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const LessThanExpression& expr) const {
    std::cout << getPrefix() << "LessThanExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const LessThanOrEqualExpression& expr) const {
    std::cout << getPrefix() << "LessThanOrEqualExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const GreaterThanExpression& expr) const {
    std::cout << getPrefix() << "GreaterThanExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const GreaterThanOrEqualExpression& expr) const {
    std::cout << getPrefix() << "GreaterThanOrEqualExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const AdditionExpression& expr) const {
    std::cout << getPrefix() << "AdditionExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const SubtractionExpression& expr) const {
    std::cout << getPrefix() << "SubtractionExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const MultiplicationExpression& expr) const {
    std::cout << getPrefix() << "MultiplicationExpression\n";
    printBinaryExpression(expr);
}

void ExpressionPrinter::operator()(const DivisionExpression& expr) const {
    std::cout << getPrefix() << "DivisionExpression\n";
    printBinaryExpression(expr);
}
void ExpressionPrinter::operator()(const SignChangeExpression&) const {}

void ExpressionPrinter::operator()(const LogicalNegationExpression&) const {}

void ExpressionPrinter::operator()(const ConversionExpression&) const {}

void ExpressionPrinter::operator()(const TypeCheckExpression&) const {}

void ExpressionPrinter::operator()(const FieldAccessExpression& expr) const {
    std::cout << getPrefix() << "FieldAccessExpression\n";
    expr.expr->accept(getSubExprPrinter());
    std::cout << getPrefix() << "└field: " << expr.field << '\n';
}

void ExpressionPrinter::operator()(const Constant& expr) const {
    std::cout << getPrefix() << "Constant: " << std::visit(ValuePrinter(), expr.value)
              << '\n';
}

void ExpressionPrinter::operator()(const FuncCall&) const {}

void ExpressionPrinter::operator()(const VariableAccess& expr) const {
    std::cout << getPrefix() << "VariableAccess " << expr.name << '\n';
}

void StatementPrinter::operator()(const IfStatement& ifStatement) {
    std::cout << getPrefix() << "IfStatement\n" << getPrefix() << "└condition:\n";
    ifStatement.condition->accept(getSubExprPrinter());
    std::cout << getPrefix() << "└statements {\n";
    for (const auto& stmt : ifStatement.statements) {
        auto vis = getSubStmtPrinter();
        stmt->accept(vis);
    }
    std::cout << getPrefix() << "}\n";
}

void StatementPrinter::operator()(const WhileStatement& whileStatement) {
    std::cout << getPrefix() << "WhileStatement\n" << getPrefix() << "└condition:\n";
    whileStatement.condition->accept(getSubExprPrinter());
    std::cout << getPrefix() << "└statements {\n";
    for (const auto& stmt : whileStatement.statements) {
        auto vis = getSubStmtPrinter();
        stmt->accept(vis);
    }
    std::cout << getPrefix() << "}\n";
}

void StatementPrinter::operator()(const ReturnStatement& stmt) {
    std::cout << getPrefix() << "ReturnStatement\n";
    if (stmt.expression)
        stmt.expression->accept(getSubExprPrinter());
}

void StatementPrinter::operator()(const PrintStatement& stmt) {
    std::cout << getPrefix() << "PrintStatement\n";
    if (stmt.expression)
        stmt.expression->accept(getSubExprPrinter());
}

void StatementPrinter::operator()(const FuncDef& funcDef) {
    std::cout << getPrefix() << "FuncDef " << funcDef.getName();
    for (const auto& param : funcDef.getParameters())
        std::cout << ' ' << param.name << ',';
    std::cout << " {\n";
    for (const auto& stmt : funcDef.getStatements()) {
        auto vis = getSubStmtPrinter();
        stmt->accept(vis);
    }
    std::cout << '\n' << getPrefix() << "}\n";
}

void StatementPrinter::operator()(const Assignment& stmt) {
    std::cout << getPrefix() << "Assignment\n"
              << std::visit(LValuePrinter(indent_ + indentWidth_), stmt.lhs) << '\n';
    stmt.rhs->accept(getSubExprPrinter());
}

void StatementPrinter::operator()(const VarDef& stmt) {
    std::cout << getPrefix() << "VarDef\n"
              << getPrefix() << "  is const: " << std::to_string(stmt.isConst) << '\n'
              << getPrefix()
              << "  type: " << std::visit(TypePrinter(indent_ + indentWidth_), stmt.type)
              << '\n'
              << getPrefix() << "  name: " << stmt.name << '\n'
              << getPrefix() << "  value:\n";
    stmt.expression->accept(getSubExprPrinter());
}

void StatementPrinter::operator()(const FuncCall& stmt) {
    std::cout << getPrefix() << "FuncCall\n"
              << getPrefix() << "  name: " << stmt.name << '\n'
              << getPrefix() << "  args:\n";
    for (const auto& arg : stmt.arguments)
        arg.value->accept(getSubExprPrinter());
}

void StatementPrinter::operator()(const StructDef& stmt) {
    std::cout << getPrefix() << "StructDef " << stmt.name << '\n'
              << getPrefix() << "fields: \n";
    for (const auto& field : stmt.fields)
        std::cout << getPrefix() << "  "
                  << std::visit(TypePrinter(indent_ + indentWidth_), field.type) << ' '
                  << field.name << '\n';
}

void StatementPrinter::operator()(const VariantDef& stmt) {
    std::cout << getPrefix() << "VariantDef " << stmt.name << '\n'
              << getPrefix() << "types:\n";
    for (const auto& type : stmt.types)
        std::cout << getPrefix() << "  "
                  << std::visit(TypePrinter(indent_ + indentWidth_), type) << '\n';
}

std::string LValuePrinter::operator()(const std::unique_ptr<FieldAccess>& lvalue) const {
    return getPrefix() + "FieldAcces\n"
           + std::visit(LValuePrinter(indent_ + indentWidth_), lvalue->container) + '\n'
           + getPrefix() + "  field: " + lvalue->field;
}

std::string LValuePrinter::operator()(const std::string& lvalue) const {
    return getPrefix() + "variable: " + lvalue;
}

std::string TypePrinter::operator()(const std::string& type) const {
    return type;
}

std::string TypePrinter::operator()(BuiltInType type) const {
    return std::string(magic_enum::enum_name(type));
}

std::string ValuePrinter::operator()(const std::monostate&) const {
    return "";
}

std::string ValuePrinter::operator()(const std::string& type) const {
    return type;
}

std::string ValuePrinter::operator()(const auto& type) const {
    return std::to_string(type);
}
