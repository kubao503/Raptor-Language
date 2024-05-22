#include "interpreter.hpp"

#include <algorithm>
#include <iostream>

#include "expr_interpreter.hpp"
#include "interpreter_errors.hpp"

struct ValueToType {
    Type operator()(Integral) { return BuiltInType::INT; }
    Type operator()(Floating) { return BuiltInType::FLOAT; }
    Type operator()(bool) { return BuiltInType::BOOL; }
    Type operator()(const std::string&) { return BuiltInType::STR; }
    Type operator()(const NamedStructObj& structObj) { return structObj.structDef->name; }
    Type operator()(const VariantObj& variantObj) { return variantObj.variantDef->name; }
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
    if (getStructDef(structDef->name))
        throw StructRedefinition{{}, structDef->name};
    if (getVariantDef(structDef->name))
        throw VariantRedefinition{{}, structDef->name};
    callStack_.top().addStruct(structDef);
}

void Interpreter::addVariant(const VariantDef* variantDef) {
    if (getVariantDef(variantDef->name))
        throw VariantRedefinition{{}, variantDef->name};
    if (getStructDef(variantDef->name))
        throw StructRedefinition{{}, variantDef->name};
    callStack_.top().addVariant(variantDef);
}

std::optional<ValueRef> Interpreter::getVariable(std::string_view name) const {
    return callStack_.top().getVariable(name);
}

std::optional<CallContext::FuncWithCtx> Interpreter::getFunctionWithCtx(
    std::string_view name) const {
    return callStack_.top().getFunctionWithCtx(name);
}

const StructDef* Interpreter::getStructDef(std::string_view name) const {
    return callStack_.top().getStructDef(name);
}

const VariantDef* Interpreter::getVariantDef(std::string_view name) const {
    return callStack_.top().getVariantDef(name);
}

ValueRef Interpreter::getValueFromExpr(const Expression& expr) {
    auto exprInterpreter = ExpressionInterpreter(*this);
    expr.accept(exprInterpreter);
    return exprInterpreter.getValue();
}

void Interpreter::operator()(const ReturnStatement& stmt) {
    returnValue_ = std::nullopt;
    if (auto expr = stmt.expression.get())
        returnValue_ = *getValueFromExpr(*expr);
}

struct ValuePrinter {
    explicit ValuePrinter(std::ostream& out)
        : out_{out} {}

    void operator()(const StructObj& s) const { printStruct(s.values); }
    void operator()(const NamedStructObj& s) const { printStruct(s.values); }
    void operator()(const VariantObj& v) const { std::visit(*this, v.valueRef->value); }
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

void Interpreter::operator()(const PrintStatement& stmt) {
    if (auto expr = stmt.expression.get()) {
        auto valueRef = getValueFromExpr(*expr);
        std::visit(ValuePrinter(out_), std::move(valueRef->value));
    }
    out_ << '\n';
}

void compareTypes(const Type& type, ValueRef valueRef) {
    if (!std::visit(TypeComparer(), type, valueRef->value))
        throw TypeMismatch{{}, type, std::visit(ValueToType(), valueRef->value)};
}

ValueRef Interpreter::convertAndCheckType(const Type& expected, ValueRef valueRef) const {
    if (auto typeName = std::get_if<std::string>(&expected)) {
        convertToUserDefinedType(*valueRef, *typeName);
    }

    compareTypes(expected, valueRef);
    return valueRef;
}

void Interpreter::convertToUserDefinedType(ValueObj& valueObj,
                                           std::string_view typeName) const {
    if (auto structDef = getStructDef(typeName))
        convertToNamedStruct(valueObj, structDef);
    else if (auto variantDef = getVariantDef(typeName))
        convertToVariant(valueObj, variantDef);
    else
        throw SymbolNotFound{{}, "User defined type", std::string(typeName)};
}

void Interpreter::convertToNamedStruct(ValueObj& valueObj,
                                       const StructDef* structDef) const {
    auto structObj = std::get_if<StructObj>(&valueObj.value);
    if (!structObj)
        return;

    if (structDef->fields.size() != structObj->values.size())
        throw InvalidFieldCount{{}, structDef->fields.size(), structObj->values.size()};

    auto binaryOp = [this](const Field& field, ValueRef value) {
        return convertAndCheckType(field.type, value);
    };
    std::ranges::transform(structDef->fields, structObj->values,
                           structObj->values.begin(), binaryOp);

    valueObj.value = NamedStructObj{structObj->values, structDef};
}

void Interpreter::convertToVariant(ValueObj& valueObj,
                                   const VariantDef* variantDef) const {
    auto compareTypeWithValue = [&](const Type& type) {
        return std::visit(TypeComparer(), type, valueObj.value);
    };
    if (std::ranges::any_of(variantDef->types, compareTypeWithValue)) {
        auto valueRef = std::make_shared<ValueObj>(valueObj);
        valueObj.value = VariantObj{std::move(valueRef), variantDef};
    }
}

void Interpreter::operator()(const VarDef& stmt) {
    auto valueRef = getValueFromExpr(*stmt.expression);
    try {
        valueRef = convertAndCheckType(stmt.type, std::move(valueRef));
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{stmt.position, e};
    } catch (const SymbolNotFound& e) {
        throw SymbolNotFound{stmt.position, e};
    } catch (const InvalidFieldCount& e) {
        throw InvalidFieldCount{stmt.position, e};
    }

    // Deliberate copy of the value
    valueRef = std::make_shared<ValueObj>(*valueRef);

    try {
        addVariable(stmt.name, std::move(valueRef));
    } catch (const VariableRedefinition& e) {
        throw VariableRedefinition{stmt.position, e};
    }
}

void Interpreter::operator()(const Assignment& stmt) {
    auto name = std::get<std::string>(stmt.lhs);
    auto oldValueRef = getVariable(name);
    if (!oldValueRef)
        throw SymbolNotFound(stmt.position, "Variable", name);
    const auto expectedType = std::visit(ValueToType(), (*oldValueRef)->value);

    auto newValueRef = getValueFromExpr(*stmt.rhs);

    try {
        newValueRef = convertAndCheckType(expectedType, std::move(newValueRef));
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

void Interpreter::operator()(const FuncCall& funcCall) {
    handleFunctionCall(funcCall);
}

std::optional<ValueObj> Interpreter::handleFunctionCall(const FuncCall& funcCall) {
    auto funcWithCtx = getFunctionWithCtx(funcCall.name);
    if (!funcWithCtx)
        throw SymbolNotFound{funcCall.position, "Function", funcCall.name};
    auto [funcDef, parentCtx] = *funcWithCtx;

    CallContext ctx{parentCtx};
    passArgumentsToCtx(ctx, funcCall.arguments, funcDef->getParameters());

    callStack_.push(std::move(ctx));

    returnValue_ = std::nullopt;
    for (const auto& stmt : funcDef->getStatements()) {
        std::visit(*this, stmt);
        if (std::holds_alternative<ReturnStatement>(stmt))
            break;
    }

    if (auto typeName = std::get_if<std::string>(&funcDef->getReturnType()))
        try {
            convertToUserDefinedType(*returnValue_, *typeName);
        } catch (const InvalidFieldCount& e) {
            throw InvalidFieldCount{funcDef->getPosition(), e};
        } catch (const TypeMismatch& e) {
            throw TypeMismatch{funcDef->getPosition(), e};
        } catch (const SymbolNotFound& e) {
            throw SymbolNotFound{funcDef->getPosition(), e};
        }

    try {
        checkReturnType(funcDef->getReturnType());
    } catch (const ReturnTypeMismatch& e) {
        throw ReturnTypeMismatch{funcDef->getPosition(), e};
    }

    callStack_.pop();
    return returnValue_;
}

void Interpreter::checkReturnType(ReturnType expected) const {
    if (std::holds_alternative<VoidType>(expected))
        expectVoidReturnValue();
    else
        expectNonVoidReturnValue(expected);
}

void Interpreter::expectVoidReturnValue() const {
    if (returnValue_) {
        auto actualType = std::visit(ValueToType(), returnValue_->value);
        throw ReturnTypeMismatch{
            {},
            VoidType(),
            std::visit([](auto t) -> ReturnType { return t; }, actualType)};
    }
}

void Interpreter::expectNonVoidReturnValue(ReturnType expected) const {
    if (!returnValue_)
        throw ReturnTypeMismatch{{}, expected, VoidType{}};

    if (!std::visit(TypeComparer(), expected, returnValue_->value)) {
        auto actualType = std::visit(ValueToType(), returnValue_->value);
        throw ReturnTypeMismatch{
            {}, expected, std::visit([](auto t) -> ReturnType { return t; }, actualType)};
    }
}

void checkArgsCount(const Arguments& args, const Parameters& params) {
    if (args.size() != params.size())
        throw std::runtime_error("Expected " + std::to_string(params.size())
                                 + " arguments but " + std::to_string(args.size())
                                 + " were provided");
}

void Interpreter::passArgumentsToCtx(CallContext& ctx, const Arguments& args,
                                     const Parameters& params) {
    checkArgsCount(args, params);

    for (std::size_t i{0}; i < args.size(); ++i)
        passArgumentToCtx(ctx, args[i], params[i]);
}

void checkArgRef(const Argument& arg, const Parameter& param) {
    if (!arg.ref && param.ref)
        throw std::runtime_error("Expected ref argument");
    if (arg.ref && !param.ref)
        throw std::runtime_error("Expected value argument");
}

void Interpreter::passArgumentToCtx(CallContext& ctx, const Argument& arg,
                                    const Parameter& param) {
    checkArgRef(arg, param);
    auto valueRef = getValueFromExpr(*arg.value);

    if (!arg.ref)
        valueRef = std::make_shared<ValueObj>(*valueRef);

    try {
        valueRef = convertAndCheckType(param.type, std::move(valueRef));
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{arg.position, e};
    } catch (const InvalidFieldCount& e) {
        throw InvalidFieldCount{arg.position, e};
    }
    ctx.addVariable(param.name, std::move(valueRef));
}

void Interpreter::operator()(const StructDef& stmt) {
    try {
        addStruct(&stmt);
    } catch (const StructRedefinition& e) {
        throw StructRedefinition{stmt.position, e};
    } catch (const VariantRedefinition& e) {
        throw VariantRedefinition{stmt.position, e};
    }
}

void Interpreter::operator()(const VariantDef& stmt) {
    try {
        addVariant(&stmt);
    } catch (const VariantRedefinition& e) {
        throw VariantRedefinition{stmt.position, e};
    } catch (const StructRedefinition& e) {
        throw StructRedefinition{stmt.position, e};
    }
}
