#include "interpreter.hpp"

#include <algorithm>
#include <iostream>

#include "expr_interpreter.hpp"

struct TypeComparer {
    bool operator()(Integral, BuiltInType variableType) {
        return variableType == BuiltInType::INT;
    }
    bool operator()(Floating, BuiltInType variableType) {
        return variableType == BuiltInType::FLOAT;
    }
    bool operator()(bool, BuiltInType variableType) {
        return variableType == BuiltInType::BOOL;
    }
    bool operator()(const std::string&, BuiltInType variableType) {
        return variableType == BuiltInType::STR;
    }
    bool operator()(const NamedStructObj& value, const std::string& variableType) {
        return value.structDef->name == variableType;
    }
    bool operator()(auto, auto) { return false; }
};

Interpreter::Interpreter(std::ostream& out)
    : out_{out} {
    callStack_.emplace(nullptr);
}

void Interpreter::interpret(const Program& program) {
    for (const auto& stmt : program.statements)
        std::visit(*this, stmt);
}

void Interpreter::addVariable(const std::string& name, ValueRef valueRef) {
    callStack_.top().addVariable(name, std::move(valueRef));
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

const StructDef* Interpreter::getStructDef(std::string_view name) const {
    return *callStack_.top().getStructDef(name);
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

    void operator()(const NamedStructObj& s) const {
        for (auto v : s.values)
            out_ << v;
    }
    void operator()(const StructObj& s) const {
        for (auto v : s.values)
            out_ << v;
    }
    void operator()(const auto& v) const { out_ << v; }

   private:
    std::ostream& out_;
};

void Interpreter::operator()(const PrintStatement& stmt) const {
    auto expr = stmt.expression.get();
    if (auto valueRef = getValueFromExpr(expr))
        std::visit(ValuePrinter(out_), std::move((*valueRef)->value));
    out_ << '\n';
}

NamedStructObj Interpreter::nameStructObj(const StructObj& structObj,
                                          std::string_view name) const {
    auto structDef = getStructDef(name);
    return {structObj.values, structDef};
}

ValueRef Interpreter::convertToNamed(const Type& type, ValueRef valueRef) const {
    if (auto structObj = std::get_if<StructObj>(&valueRef->value)) {
        auto name = std::get_if<std::string>(&type);
        if (!name)
            throw std::runtime_error("Expression type does not match with variable type");
        return std::make_shared<ValueObj>(nameStructObj(*structObj, *name));
    }
    return valueRef;
}

ValueRef Interpreter::convertToNamed(ValueRef oldValueRef, ValueRef newValueRef) const {
    if (auto structObj = std::get_if<StructObj>(&newValueRef->value)) {
        auto oldNamedStructObj = std::get_if<NamedStructObj>(&oldValueRef->value);
        if (!oldNamedStructObj)
            throw std::runtime_error("Expression type does not match with variable type");
        auto namedStructObj =
            NamedStructObj{std::move(structObj->values), oldNamedStructObj->structDef};
        return std::make_shared<ValueObj>(std::move(namedStructObj));
    }
    return newValueRef;
}

void Interpreter::operator()(const VarDef& stmt) {
    auto valueRef = getValueFromExpr(stmt.expression.get());
    auto namedValueRef = convertToNamed(stmt.type, std::move(*valueRef));

    if (!std::visit(TypeComparer(), namedValueRef->value, stmt.type))
        throw std::runtime_error("Expression type does not match with variable type");

    addVariable(stmt.name, std::move(namedValueRef));
}

void Interpreter::operator()(const Assignment& stmt) const {
    auto name = std::get<std::string>(stmt.lhs);
    auto oldValueRef = readVariable(name);

    auto newValueRef = getValueFromExpr(stmt.rhs.get()).value();
    auto newNamedValue = convertToNamed(oldValueRef, newValueRef);

    if (oldValueRef->value.index() != newNamedValue->value.index())
        throw std::runtime_error("Mismatched types at assignment");

    oldValueRef->value = std::move(newNamedValue->value);
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

    if (!arg.ref)
        valueRef = std::make_shared<ValueObj>(**valueRef);

    auto namedValueRef = convertToNamed(param.type, *valueRef);

    if (!std::visit(TypeComparer(), namedValueRef->value, param.type))
        throw std::runtime_error("Argument type does not match with parameter type");

    callStack_.top().addVariable(param.name, std::move(namedValueRef));
}

void Interpreter::operator()(const StructDef& stmt) {
    addStruct(&stmt);
}
