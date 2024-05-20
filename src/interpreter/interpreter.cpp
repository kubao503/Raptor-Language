#include "interpreter.hpp"

#include <algorithm>
#include <iostream>

#include "expr_interpreter.hpp"
#include "interpreter_errors.hpp"

struct TypeComparer {
    bool operator()(BuiltInType variableType, Integral) {
        return variableType == BuiltInType::INT;
    }
    bool operator()(BuiltInType variableType, Floating) {
        return variableType == BuiltInType::FLOAT;
    }
    bool operator()(BuiltInType variableType, bool) {
        return variableType == BuiltInType::BOOL;
    }
    bool operator()(BuiltInType variableType, const std::string&) {
        return variableType == BuiltInType::STR;
    }
    bool operator()(const std::string& variableType, const NamedStructObj& value) {
        return value.structDef->name == variableType;
    }
    bool operator()(auto, auto) { return false; }
};

struct ValueToType {
    Type operator()(Integral) { return BuiltInType::INT; }
    Type operator()(Floating) { return BuiltInType::FLOAT; }
    Type operator()(bool) { return BuiltInType::BOOL; }
    Type operator()(const std::string&) { return BuiltInType::STR; }
    Type operator()(const NamedStructObj& structObj) { return structObj.structDef->name; }
    Type operator()(const StructObj&) { return "Anonymous struct"; }
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
    explicit ValuePrinter(std::ostream& out)
        : out_{out} {}

    void operator()(const NamedStructObj& s) const { printStruct(s.values); }
    void operator()(const StructObj& s) const { printStruct(s.values); }
    void operator()(const auto& v) const { out_ << std::boolalpha << v; }

   private:
    void printStruct(const std::vector<ValueRef>& values) const {
        out_ << "{ ";
        for (auto v : values) {
            std::visit(*this, v->value);
            out_ << ' ';
        }
        out_ << '}';
    }

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

void compareTypes(ValueRef oldValueRef, ValueRef newValueRef) {
    if (oldValueRef->value.index() != newValueRef->value.index())
        throw TypeMismatch{{},
                           std::visit(ValueToType(), oldValueRef->value),
                           std::visit(ValueToType(), newValueRef->value)};
}

void compareTypes(const Type& type, ValueRef valueRef) {
    if (!std::visit(TypeComparer(), type, valueRef->value))
        throw TypeMismatch{{}, type, std::visit(ValueToType(), valueRef->value)};
}

ValueRef Interpreter::checkTypeAndConvert(ValueRef oldValueRef,
                                          ValueRef newValueRef) const {
    auto structObj = std::get_if<StructObj>(&newValueRef->value);
    auto oldNamedStructObj = std::get_if<NamedStructObj>(&oldValueRef->value);

    if (structObj && oldNamedStructObj) {
        if (oldNamedStructObj->values.size() != structObj->values.size())
            throw InvalidFieldCount{
                {}, oldNamedStructObj->values.size(), structObj->values.size()};

        auto binaryOp = [this](ValueRef oldValue, ValueRef newValue) {
            return checkTypeAndConvert(oldValue, newValue);
        };
        std::ranges::transform(oldNamedStructObj->values, structObj->values,
                               structObj->values.begin(), binaryOp);

        auto namedStructObj =
            NamedStructObj{std::move(structObj->values), oldNamedStructObj->structDef};
        newValueRef = std::make_shared<ValueObj>(std::move(namedStructObj));
    }

    compareTypes(oldValueRef, newValueRef);
    return newValueRef;
}

ValueRef Interpreter::checkTypeAndConvert(const Type& type, ValueRef valueRef) const {
    auto structObj = std::get_if<StructObj>(&valueRef->value);
    auto name = std::get_if<std::string>(&type);

    if (structObj && name) {
        auto structDef = getStructDef(*name);
        if (!structDef)
            throw SymbolNotFound{{}, "Struct definition", *name};

        if ((*structDef)->fields.size() != structObj->values.size())
            throw InvalidFieldCount{
                {}, (*structDef)->fields.size(), structObj->values.size()};

        auto binaryOp = [this](const Field& field, ValueRef value) {
            return checkTypeAndConvert(field.type, value);
        };
        std::ranges::transform((*structDef)->fields, structObj->values,
                               structObj->values.begin(), binaryOp);

        auto namedStructObj = NamedStructObj{structObj->values, *structDef};
        valueRef = std::make_shared<ValueObj>(namedStructObj);
    }

    compareTypes(type, valueRef);
    return valueRef;
}

void Interpreter::operator()(const VarDef& stmt) {
    auto valueRef = getValueFromExpr(*stmt.expression);
    try {
        valueRef = checkTypeAndConvert(stmt.type, std::move(valueRef));
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{stmt.position, e};
    } catch (const SymbolNotFound& e) {
        throw SymbolNotFound{stmt.position, e};
    } catch (const InvalidFieldCount& e) {
        throw InvalidFieldCount{stmt.position, e};
    }

    try {
        addVariable(stmt.name, std::move(valueRef));
    } catch (const VariableRedefinition& e) {
        throw VariableRedefinition{stmt.position, e};
    }
}

void Interpreter::operator()(const Assignment& stmt) const {
    auto name = std::get<std::string>(stmt.lhs);
    auto oldValueRef = getVariable(name);
    if (!oldValueRef)
        throw SymbolNotFound(stmt.position, "Variable", name);

    auto newValueRef = getValueFromExpr(*stmt.rhs);

    try {
        newValueRef = checkTypeAndConvert(*oldValueRef, std::move(newValueRef));
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{stmt.position, e};
    } catch (const InvalidFieldCount& e) {
        throw InvalidFieldCount{stmt.position, e};
    }

    (*oldValueRef)->value = std::move(newValueRef->value);
}

void Interpreter::operator()(const FuncDef& stmt) {
    try {
        addFunction(&stmt);
    } catch (const FunctionRedefinition& e) {
        throw FunctionRedefinition{stmt.getPosition(), e};
    }
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
        passArgument(args[i], params[i]);
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

    try {
        valueRef = checkTypeAndConvert(param.type, std::move(valueRef));
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{arg.position, e};
    } catch (const InvalidFieldCount& e) {
        throw InvalidFieldCount{arg.position, e};
    }
    callStack_.top().addVariable(param.name, std::move(valueRef));
}

void Interpreter::operator()(const StructDef& stmt) {
    addStruct(&stmt);
}
