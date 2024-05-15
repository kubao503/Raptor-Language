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

void Interpreter::addFunction(const FuncDef* funcDef) {
    callStack_.top().addFunction(funcDef);
}

void Interpreter::addStruct(const StructDef* structDef) {
    callStack_.top().addStruct(structDef);
}

ValueRef Interpreter::readVariable(std::string_view name) const {
    return *callStack_.top().readVariable(name);
}

CallContext::FuncWithCtx Interpreter::getFunctionWithCtx(std::string_view name) const {
    return *callStack_.top().getFunctionWithCtx(name);
}

std::optional<ValueRef> Interpreter::getValueFromExpr(const Expression* expr) const {
    if (!expr)
        return std::nullopt;
    auto exprInterpreter = ExpressionInterpreter(*this);
    expr->accept(exprInterpreter);
    return exprInterpreter.getValue();
}

struct ValuePrinter {
    ValuePrinter(std::ostream& out)
        : out_{out} {}

    void operator()(const StructObj&) const {}
    void operator()(const auto& type) const { out_ << std::boolalpha << type; }

   private:
    std::ostream& out_;
};

void Interpreter::operator()(const PrintStatement& stmt) const {
    auto expr = stmt.expression.get();
    if (auto valueRef = getValueFromExpr(expr))
        std::visit(ValuePrinter(out_), std::move((*valueRef)->value));
    out_ << '\n';
}

void Interpreter::operator()(const VarDef& stmt) {
    auto valueRef = getValueFromExpr(stmt.expression.get());
    addVariable(stmt.name, *valueRef);
}

void Interpreter::operator()(const Assignment& stmt) const {
    auto name = std::get<std::string>(stmt.lhs);
    auto valueRef = readVariable(name);
    auto newValue = getValueFromExpr(stmt.rhs.get()).value();

    if (valueRef->type != newValue->type)
        throw std::runtime_error("Mismatched types at assignment");

    valueRef->value = std::move(newValue->value);
}

void Interpreter::operator()(const FuncDef& stmt) {
    addFunction(&stmt);
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
    for (std::size_t i{0}; i < args.size(); ++i)
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
    auto valueRef = getValueFromExpr(arg.value.get());

    if ((*valueRef)->type != param.type)
        throw std::runtime_error("Argument type does not match with parameter type");

    if (!arg.ref)
        valueRef = std::make_shared<ValueObj>(**valueRef);

    callStack_.top().addVariable(param.name, std::move(*valueRef));
}

void Interpreter::operator()(const StructDef& stmt) {
    addStruct(&stmt);
}
