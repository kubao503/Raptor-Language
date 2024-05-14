#include "interpreter.hpp"

#include <algorithm>
#include <iostream>

#include "expr_interpreter.hpp"

Interpreter::Interpreter(std::ostream& out)
    : out_{out} {
    callStack_.emplace(nullptr);
}

void Interpreter::interpret(const Program& program) {
    for (const auto& stmt : program.statements)
        std::visit(*this, stmt);
}

void Interpreter::addVariable(const std::string& name, ValueRef ref) {
    callStack_.top().addVariable(name, ref);
}

void Interpreter::addFunction(const std::string& name, const FuncDef* func) {
    callStack_.top().addFunction(name, func);
}

ValueRef Interpreter::readVariable(std::string_view name) const {
    return *callStack_.top().readVariable(name);
}

CallContext::FuncWithCtx Interpreter::getFunctionWithCtx(std::string_view name) const {
    return *callStack_.top().getFunctionWithCtx(name);
}

std::optional<Value> Interpreter::getValueFromExpr(const Expression* expr) const {
    if (!expr)
        return std::nullopt;
    auto exprInterpreter = ExpressionInterpreter(*this);
    expr->accept(exprInterpreter);
    return exprInterpreter.getValue();
}

void Interpreter::operator()(const PrintStatement& stmt) const {
    auto expr = stmt.expression.get();
    if (auto value = getValueFromExpr(expr))
        std::visit(ValuePrinter(out_), std::move(*value));

    out_ << '\n';
}

void Interpreter::operator()(const VarDef& stmt) {
    auto value = getValueFromExpr(stmt.expression.get());
    auto valueRef = std::make_shared<ValueObj>(std::move(*value));
    addVariable(stmt.name, valueRef);
}

void Interpreter::operator()(const Assignment& stmt) const {
    auto name = std::get<std::string>(stmt.lhs);
    auto valueRef = readVariable(name);
    valueRef->value = getValueFromExpr(stmt.rhs.get()).value();
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
    for (size_t i{0}; i < args.size(); ++i)
        passArgument(args.at(i), params.at(i));
}

void checkArgRef(const Argument& arg, const Parameter& param) {
    if (!arg.ref && param.ref)
        throw std::runtime_error("Expected ref argument");
    if (arg.ref && !param.ref)
        throw std::runtime_error("Expected value argument");
}

void Interpreter::passArgument(const Argument& arg, const Parameter& param) {
    checkArgRef(arg, param);
    auto value = getValueFromExpr(arg.value.get());

    if (std::visit(ValueToType(), *value) != param.type)
        throw std::runtime_error("Argument type does not match");

    auto valueRef = std::make_shared<ValueObj>(std::move(*value));
    if (arg.ref) {
        auto varAccess = dynamic_cast<VariableAccess*>(arg.value.get());
        if (!varAccess)
            throw std::runtime_error("Expected variable after ref keyword");
        auto name = varAccess->name;
        valueRef = readVariable(name);
    }
    callStack_.top().addVariable(param.name, std::move(valueRef));
}

void ValuePrinter::operator()(const auto& type) const {
    out_ << std::boolalpha << type;
}
