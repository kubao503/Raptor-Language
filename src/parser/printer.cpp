#include "printer.hpp"

std::ostream& operator<<(std::ostream& stream, const Program& program) {
    for (const auto& stmt : program.statements) {
        stream << std::visit(StatementPrinter(), stmt) << '\n';
    }
    return stream;
}

ExpressionPrinter BasePrinter::getSubExprPrinter() const {
    return ExpressionPrinter(indent_ + indentWidth_);
}

StatementPrinter BasePrinter::getSubStmtPrinter() const {
    return StatementPrinter(indent_ + indentWidth_);
}

std::string ExpressionPrinter::printBinaryExpression(const auto& expression) const {
    return std::visit(getSubExprPrinter(), expression->lhs) + '\n'
           + std::visit(getSubExprPrinter(), expression->rhs);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<DisjunctionExpression>& disjunction) const {
    return getPrefix() + "DisjunctionExpression\n" + printBinaryExpression(disjunction);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<ConjunctionExpression>& conjunction) const {
    return getPrefix() + "ConjunctionExpression\n" + printBinaryExpression(conjunction);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<EqualExpression>& expr) const {
    return getPrefix() + "EqualExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<NotEqualExpression>& expr) const {
    return getPrefix() + "NotEqualExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<LessThanExpression>& expr) const {
    return getPrefix() + "LessThanExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<LessThanOrEqualExpression>& expr) const {
    return getPrefix() + "LessThanOrEqualExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<GreaterThanExpression>& expr) const {
    return getPrefix() + "GreaterThanExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<GreaterThanOrEqualExpression>& expr) const {
    return getPrefix() + "GreaterThanOrEqualExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<AdditionExpression>& expr) const {
    return getPrefix() + "AdditionExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<SubtractionExpression>& expr) const {
    return getPrefix() + "SubtractionExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<MultiplicationExpression>& expr) const {
    return getPrefix() + "MultiplicationExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<DivisionExpression>& expr) const {
    return getPrefix() + "DivisionExpression\n" + printBinaryExpression(expr);
}

std::string ExpressionPrinter::operator()(const auto& expr) const {
    return getPrefix() + typeid(expr).name();
}

std::string StatementPrinter::printIfOrWhileStatement(
    const IfOrWhileStatement& ifOrWhile) const {
    std::string output = getPrefix() + "└condition:\n"
                         + std::visit(getSubExprPrinter(), ifOrWhile.condition) + '\n'
                         + getPrefix() + "└statements {";
    for (const auto& stmt : ifOrWhile.statements)
        output += '\n' + std::visit(getSubStmtPrinter(), stmt);
    return output + '\n' + getPrefix() + '}';
}

std::string StatementPrinter::operator()(const IfStatement& ifStatement) const {
    return getPrefix() + "IfStatement\n" + printIfOrWhileStatement(ifStatement);
}

std::string StatementPrinter::operator()(const WhileStatement& whileStatement) const {
    return getPrefix() + "WhileStatement\n" + printIfOrWhileStatement(whileStatement);
}

std::string StatementPrinter::operator()(const FuncDef& funcDef) const {
    std::string output = getPrefix() + "FuncDef " + funcDef.getName();
    for (const auto& param : funcDef.getParameters()) output += ' ' + param.name + ',';
    output += " {";
    for (const auto& stmt : funcDef.getStatements())
        output += '\n' + std::visit(getSubStmtPrinter(), stmt);
    return output + '\n' + getPrefix() + '}';
}

std::string StatementPrinter::operator()(const Assignment&) const {
    return getPrefix() + "Assignment";
}

std::string StatementPrinter::operator()(const auto& stmt) const {
    return getPrefix() + typeid(stmt).name();
}
