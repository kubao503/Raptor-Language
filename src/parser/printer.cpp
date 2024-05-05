#include "printer.hpp"

std::ostream& operator<<(std::ostream& stream, const Program& program) {
    for (const auto& stmt : program.statements) {
        stream << std::visit(StatementPrinter(), stmt) << '\n';
    }
    return stream;
}

std::string StatementPrinter::operator()(const IfStatement& ifStatement) const {
    std::string output = getPrefix() + "IfStatement ";
    for (const auto& stmt : ifStatement.statements)
        output += '\n' + getPrefix() + std::visit(StatementPrinter(indent_ + 2), stmt);
    return output;
}

std::string StatementPrinter::operator()(const WhileStatement& whileStatement) const {
    std::string output = getPrefix() + "WhileStatement ";
    for (const auto& stmt : whileStatement.statements)
        output += '\n' + getPrefix() + std::visit(StatementPrinter(indent_ + 2), stmt);
    return output;
}

std::string StatementPrinter::operator()(const FuncDef& funcDef) const {
    std::string output = getPrefix() + "FuncDef " + funcDef.getName();
    for (const auto& param : funcDef.getParameters()) output += ' ' + param.name + ',';
    for (const auto& stmt : funcDef.getStatements())
        output += '\n' + getPrefix() + std::visit(StatementPrinter(indent_ + 2), stmt);
    return output;
}

std::string StatementPrinter::operator()(const Assignment&) const {
    return getPrefix() + "Assignment";
}

std::string StatementPrinter::operator()(const auto& stmt) const {
    return getPrefix() + typeid(stmt).name();
}
