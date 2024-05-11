#include "interpreter.hpp"

#include <algorithm>
#include <iostream>

void Interpreter::interpret() {
    for (const auto& stmt : program_.statements)
        std::visit(*this, stmt);
}

ExpressionInterpreter::ExpressionInterpreter(const Interpreter& interpreter)
    : interpreter_{interpreter} {}

Value ExpressionInterpreter::operator()(const Constant& expr) const {
    return expr.value;
}

void Interpreter::operator()(const PrintStatement& stmt) const {
    const auto value = std::visit(ExpressionInterpreter(*this), *stmt.expression);
    std::visit(ValuePrinter(), value);
    std::cout << '\n';
}

void Interpreter::operator()(const FuncDef& stmt) {
    functions_.push_back({stmt.getName(), &stmt});
}

void Interpreter::operator()(const FuncCall& stmt) const {
    auto res = std::ranges::find(functions_, stmt.name, &Pair::first);
    auto func = res->second;
    for (const auto& stmt : func->getStatements())
        std::visit(*this, stmt);
}

void ValuePrinter::operator()(const auto& type) const {
    std::cout << std::boolalpha << type;
}
