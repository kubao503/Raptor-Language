#include "interpreter.hpp"

#include <algorithm>
#include <iostream>

Interpreter::Interpreter(const Program& program, std::ostream& out)
    : program_{program}, out_{out} {
    callStack_.emplace(nullptr);
    interpret();
}

void Interpreter::interpret() {
    for (const auto& stmt : program_.statements)
        std::visit(*this, stmt);
}

void Interpreter::addVariable(const std::string& name, ValueRef ref) {
    callStack_.top().addVariable(name, ref);
}

void Interpreter::addFunction(const std::string& name, const FuncDef* func) {
    callStack_.top().addFunction(name, func);
}

Value Interpreter::readVariable(std::string_view name) const {
    return *callStack_.top().readVariable(name);
}

CallContext::FuncWithCtx Interpreter::getFunctionWithCtx(std::string_view name) const {
    return *callStack_.top().getFunctionWithCtx(name);
}

ExpressionInterpreter::ExpressionInterpreter(const Interpreter& interpreter)
    : interpreter_{interpreter} {}

Value ExpressionInterpreter::operator()(const Constant& expr) const {
    return expr.value;
}

Value ExpressionInterpreter::operator()(const VariableAccess& expr) const {
    return interpreter_.readVariable(expr.name);
}

void Interpreter::operator()(const PrintStatement& stmt) const {
    const auto value = std::visit(ExpressionInterpreter(*this), *stmt.expression);
    std::visit(ValuePrinter(out_), value);
    out_ << '\n';
}

void Interpreter::operator()(const VarDef& stmt) {
    auto value = std::visit(ExpressionInterpreter(*this), stmt.expression);
    auto valueRef = std::make_shared<ValueObj>(std::move(value));
    addVariable(stmt.name, valueRef);
    values_.push_back(std::move(valueRef));
}

void Interpreter::operator()(const FuncDef& stmt) {
    addFunction(stmt.getName(), &stmt);
}

void checkArgsCount(const Arguments& args, const Parameters& params) {
    if (args.size() != params.size())
        throw std::runtime_error("Expected " + std::to_string(params.size())
                                 + " arguments but " + std::to_string(args.size())
                                 + " were provided");
}

void Interpreter::operator()(const FuncCall& stmt) {
    auto [func, ctx] = getFunctionWithCtx(stmt.name);

    checkArgsCount(stmt.arguments, func->getParameters());
    callStack_.emplace(ctx);
    passArguments(stmt.arguments, func->getParameters());

    for (const auto& stmt : func->getStatements())
        std::visit(*this, stmt);
    callStack_.pop();
}

void Interpreter::passArguments(const Arguments& args, const Parameters& params) {
    for (size_t i{0}; i < args.size(); ++i) {
        auto value = std::visit(ExpressionInterpreter(*this), args.at(i).value);
        auto valueRef = std::make_shared<ValueObj>(std::move(value));
        callStack_.top().addVariable(params.at(i).name, std::move(valueRef));
    }
}

void ValuePrinter::operator()(const auto& type) const {
    out_ << std::boolalpha << type;
}
