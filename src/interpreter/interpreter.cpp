#include "interpreter.hpp"

#include <iostream>

void Interpreter::interpret() {
    for (const auto& stmt : program_.statements)
        std::visit(*this, stmt);
}

Value ExpressionInterpreter::operator()(const Constant& expr) const {
    return expr.value;
}

void Interpreter::operator()(const PrintStatement& stmt) const {
    const auto value = std::visit(ExpressionInterpreter(), *stmt.expression);
    std::visit(ValuePrinter(), value);
    std::cout << '\n';
}

void Interpreter::operator()(const FuncDef& stmt) {
    functions_.push_back({stmt.getName(), &stmt});
}

void ValuePrinter::operator()(const auto& type) const {
    std::cout << std::boolalpha << type;
}
