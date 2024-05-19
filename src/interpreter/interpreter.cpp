#include "interpreter.hpp"

#include <algorithm>
#include <iostream>

#include "expr_interpreter.hpp"
#include "interpreter_errors.hpp"

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

std::optional<ValueRef> Interpreter::getVariable(std::string_view name) const {
    return callStack_.top().getVariable(name);
}

std::optional<CallContext::FuncWithCtx> Interpreter::getFunctionWithCtx(
    std::string_view name) const {
    return callStack_.top().getFunctionWithCtx(name);
}

std::optional<const StructDef*> Interpreter::getStructDef(std::string_view name) const {
    return callStack_.top().getStructDef(name);
}

ValueRef Interpreter::getValueFromExpr(const Expression& expr) const {
    auto exprInterpreter = ExpressionInterpreter(*this);
    expr.accept(exprInterpreter);
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
    if (expr) {
        auto valueRef = getValueFromExpr(*expr);
        std::visit(ValuePrinter(out_), std::move(valueRef->value));
    }
    out_ << '\n';
}

ValueRef Interpreter::checkTypeAndConvert(const Type& type, ValueRef valueRef) const {
    auto name = std::get_if<std::string>(&type);
    auto structObj = std::get_if<StructObj>(&valueRef->value);
    if (structObj && name) {
        auto structDef = getStructDef(*name);
        if (!structDef)
            throw SymbolNotFound{{}, "Struct definition", *name};
        auto namedStructObj = NamedStructObj{structObj->values, *structDef};
        valueRef = std::make_shared<ValueObj>(namedStructObj);
    }

    if (!std::visit(TypeComparer(), valueRef->value, type))
        throw std::runtime_error("Expression type does not match with variable type");

    return valueRef;
}

ValueRef Interpreter::checkTypeAndConvert(ValueRef oldValueRef,
                                          ValueRef newValueRef) const {
    auto structObj = std::get_if<StructObj>(&newValueRef->value);
    auto oldNamedStructObj = std::get_if<NamedStructObj>(&oldValueRef->value);
    if (structObj && oldNamedStructObj) {
        auto namedStructObj =
            NamedStructObj{std::move(structObj->values), oldNamedStructObj->structDef};
        newValueRef = std::make_shared<ValueObj>(std::move(namedStructObj));
    }

    if (oldValueRef->value.index() != newValueRef->value.index())
        throw std::runtime_error("Argument type does not match with parameter type");

    return newValueRef;
}

void Interpreter::operator()(const VarDef& stmt) {
    auto valueRef = getValueFromExpr(*stmt.expression);
    auto namedValueRef = checkTypeAndConvert(stmt.type, std::move(valueRef));

    addVariable(stmt.name, std::move(namedValueRef));
}

void Interpreter::operator()(const Assignment& stmt) const {
    auto name = std::get<std::string>(stmt.lhs);
    auto oldValueRef = getVariable(name);
    if (!oldValueRef)
        throw SymbolNotFound(stmt.position, "Variable", name);

    auto newValueRef = getValueFromExpr(*stmt.rhs);
    auto newNamedValue = checkTypeAndConvert(*oldValueRef, std::move(newValueRef));

    (*oldValueRef)->value = std::move(newNamedValue->value);
}

void Interpreter::operator()(const FuncDef& stmt) {
    addFunction(&stmt);
}

void Interpreter::operator()(const FuncCall& stmt) {
    auto funcWithCtx = getFunctionWithCtx(stmt.name);
    if (!funcWithCtx)
        throw SymbolNotFound{stmt.position, "Function", stmt.name};
    auto [func, ctx] = *funcWithCtx;

    callStack_.emplace(ctx);
    passArguments(stmt.arguments, func->getParameters());

    for (const auto& stmt : func->getStatements())
        std::visit(*this, stmt);
    callStack_.pop();
}

void checkArgsCount(const Arguments& args, const Parameters& params) {
    if (args.size() != params.size())
        throw std::runtime_error("Expected " + std::to_string(params.size())
                                 + " arguments but " + std::to_string(args.size())
                                 + " were provided");
}

void Interpreter::passArguments(const Arguments& args, const Parameters& params) {
    checkArgsCount(args, params);

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
    auto valueRef = getValueFromExpr(*arg.value);

    if (!arg.ref)
        valueRef = std::make_shared<ValueObj>(*valueRef);

    auto namedValueRef = checkTypeAndConvert(param.type, std::move(valueRef));
    callStack_.top().addVariable(param.name, std::move(namedValueRef));
}

void Interpreter::operator()(const StructDef& stmt) {
    addStruct(&stmt);
}
