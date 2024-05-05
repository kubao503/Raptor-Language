#include "printer.hpp"

std::ostream& operator<<(std::ostream& stream, const Program& program) {
    for (const auto& stmt : program.statements) {
        stream << std::visit(StatementPrinter(), stmt) << '\n';
    }
    return stream;
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<DisjunctionExpression>& disjunction) const {
    return getPrefix() + "DisjunctionExpression\n"
           + std::visit(ExpressionPrinter(indent_ + indentWidth_), disjunction->lhs)
           + '\n'
           + std::visit(ExpressionPrinter(indent_ + indentWidth_), disjunction->rhs);
}

std::string ExpressionPrinter::operator()(
    const std::unique_ptr<ConjunctionExpression>& conjunction) const {
    return getPrefix() + "ConjunctionExpression\n"
           + std::visit(ExpressionPrinter(indent_ + indentWidth_), conjunction->lhs)
           + '\n'
           + std::visit(ExpressionPrinter(indent_ + indentWidth_), conjunction->rhs);
}

std::string ExpressionPrinter::operator()(const auto& expr) const {
    return getPrefix() + typeid(expr).name();
}

std::string StatementPrinter::operator()(const IfStatement& ifStatement) const {
    std::string output = getPrefix() + "IfStatement ";
    for (const auto& stmt : ifStatement.statements)
        output += '\n' + std::visit(StatementPrinter(indent_ + indentWidth_), stmt);
    return output;
}

std::string StatementPrinter::operator()(const WhileStatement& whileStatement) const {
    std::string output =
        getPrefix() + "WhileStatement\n" + getPrefix() + "└condition:\n"
        + std::visit(ExpressionPrinter(indent_ + indentWidth_), whileStatement.condition)
        + '\n' + getPrefix() + "└statements {";
    for (const auto& stmt : whileStatement.statements)
        output += '\n' + std::visit(StatementPrinter(indent_ + indentWidth_), stmt);
    return output + '\n' + getPrefix() + '}';
}

std::string StatementPrinter::operator()(const FuncDef& funcDef) const {
    std::string output = getPrefix() + "FuncDef " + funcDef.getName();
    for (const auto& param : funcDef.getParameters()) output += ' ' + param.name + ',';
    output += " }";
    for (const auto& stmt : funcDef.getStatements())
        output += '\n' + std::visit(StatementPrinter(indent_ + indentWidth_), stmt);
    return output + '\n' + getPrefix() + '}';
}

std::string StatementPrinter::operator()(const Assignment&) const {
    return getPrefix() + "Assignment";
}

std::string StatementPrinter::operator()(const auto& stmt) const {
    return getPrefix() + typeid(stmt).name();
}
