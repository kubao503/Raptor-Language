#include "interpreter.hpp"

#include <iostream>

void interpret(const Program& program) {
    for (const auto& stmt : program.statements)
        std::visit(Interpreter(), stmt);
}

Value ExpressionInterpreter::operator()(const Constant& expr) const {
    return expr.value;
}

void Interpreter::operator()(const PrintStatement& stmt) const {
    const Value& value{std::visit(ExpressionInterpreter(), *stmt.expression)};
    std::visit(ValuePrinter(), value);
    std::cout << '\n';
}

void ValuePrinter::operator()(const std::string& type) const {
    std::cout << type;
}

void ValuePrinter::operator()(const auto& type) const {
    std::cout << std::to_string(type);
}
