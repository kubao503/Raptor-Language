#include "interpreter.hpp"

#include <algorithm>
#include <iostream>

#include "expr_interpreter.hpp"
#include "interpreter_errors.hpp"

Interpreter::Interpreter(std::ostream& out)
    : out_{out} {
    callStack_.emplace(nullptr);
}

void Interpreter::interpret(const Program& program) {
    for (const auto& stmt : program.statements) {
        stmt->accept(*this);
        if (returning_)
            throw ReturnTypeMismatch{stmt->position, "No return in global scope",
                                     "Returning in global scope"};
    }
}

void Interpreter::addVariable(VarEntry entry) {
    callStack_.top().addVariable(std::move(entry));
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

std::optional<RefObj> Interpreter::getVariable(std::string_view name) const {
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

ValueHolder Interpreter::getValueFromExpr(const Expression& expr) {
    auto exprInterpreter = ExpressionInterpreter(this);
    expr.accept(exprInterpreter);
    return exprInterpreter.getLastResult();
}

template <typename ConditionalStatement>
bool Interpreter::evaluateCondition(const ConditionalStatement& stmt) {
    const auto conditionValue = getHeldValue(getValueFromExpr(*stmt.condition));
    const auto condition = std::get_if<bool>(&conditionValue.value);
    if (!condition)
        throw TypeMismatch{stmt.condition->position, BuiltInType::BOOL,
                           std::visit(ValueToType(), conditionValue.value)};
    return *condition;
}

void Interpreter::interpretStatements(const Statements& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
        if (returning_)
            break;
    }
}

void Interpreter::operator()(const IfStatement& ifStmt) {
    const auto condition = evaluateCondition(ifStmt);

    if (condition) {
        callStack_.top().addScope();
        interpretStatements(ifStmt.statements);
        callStack_.top().removeScope();
    }
}

void Interpreter::operator()(const WhileStatement& whileStmt) {
    while (evaluateCondition(whileStmt) && !returning_) {
        callStack_.top().addScope();
        interpretStatements(whileStmt.statements);
        callStack_.top().removeScope();
    }
}

void Interpreter::operator()(const ReturnStatement& stmt) {
    if (auto expr = stmt.expression.get()) {
        auto heldValue = getHeldValueCopy(getValueFromExpr(*expr));
        returnValue_ = std::move(heldValue);
    }
    returning_ = true;
}

struct ValuePrinter {
    explicit ValuePrinter(std::ostream& out)
        : out_{out} {}

    void operator()(const StructObj& s) const { printStruct(s.values); }
    void operator()(const NamedStructObj& s) const { printStruct(s.values); }
    void operator()(const VariantObj& v) const { std::visit(*this, v.valueObj->value); }
    void operator()(const auto& v) const { out_ << std::boolalpha << v; }

   private:
    void printStruct(const StructObj::Values& values) const {
        out_ << "{ ";
        for (const auto& v : values) {
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
        const auto valueObj = getHeldValue(std::move(valueRef));
        std::visit(ValuePrinter(out_), valueObj.value);
    }
    out_ << '\n';
}

void expectVoidReturnValue(const ReturnValue& valueObj) {
    if (valueObj) {
        const auto actualType = std::visit(ValueToType(), valueObj->value);
        throw ReturnTypeMismatch{
            {},
            VoidType(),
            std::visit([](auto t) -> ReturnType { return t; }, actualType)};
    }
}

void expectNonVoidReturnValue(const ReturnType& expected, const ReturnValue& valueObj) {
    if (!valueObj)
        throw ReturnTypeMismatch{{}, expected, VoidType{}};

    if (!std::visit(TypeComparer(), expected, valueObj->value)) {
        const auto actualType = std::visit(ValueToType(), valueObj->value);
        throw ReturnTypeMismatch{
            {}, expected, std::visit([](auto t) -> ReturnType { return t; }, actualType)};
    }
}

void checkReturnType(const ReturnType& expected, const ReturnValue& valueObj) {
    if (std::holds_alternative<VoidType>(expected))
        expectVoidReturnValue(valueObj);
    else
        expectNonVoidReturnValue(expected, valueObj);
}

void checkValueType(const Type& type, const ValueObj& valueObj) {
    if (!std::visit(TypeComparer(), type, valueObj.value))
        throw TypeMismatch{{}, type, std::visit(ValueToType(), valueObj.value)};
}

ValueHolder Interpreter::convertAndCheckType(const Type& expected,
                                             ValueHolder valueRef) const {
    auto valueObj = getHeldValueCopy(std::move(valueRef));
    auto userDefinedTypeName = std::get_if<std::string>(&expected);
    if (!std::visit(TypeComparer(), expected, valueObj.value) && userDefinedTypeName) {
        convertToUserDefinedType(valueObj, *userDefinedTypeName);
        checkValueType(expected, valueObj);
        return valueObj;
    }

    checkValueType(expected, getHeldValueCopy(valueRef));
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

    auto binaryOp = [this](const Field& field, std::unique_ptr<ValueObj> valueObj) {
        auto convertedValue = convertAndCheckType(field.type, std::move(*valueObj));
        auto convertedValueObj = getHeldValue(std::move(convertedValue));
        return std::make_unique<ValueObj>(std::move(convertedValueObj));
    };
    std::ranges::transform(structDef->fields.begin(), structDef->fields.end(),
                           std::make_move_iterator(structObj->values.begin()),
                           std::make_move_iterator(structObj->values.end()),
                           structObj->values.begin(), binaryOp);

    valueObj.value = NamedStructObj{std::move(structObj->values), structDef};
}

void Interpreter::convertToVariant(ValueObj& valueObj,
                                   const VariantDef* variantDef) const {
    auto compareTypeWithValue = [&](const Type& type) {
        return std::visit(TypeComparer(), type, valueObj.value);
    };
    if (std::ranges::any_of(variantDef->types, compareTypeWithValue)) {
        auto valuePtr = std::make_unique<ValueObj>(std::move(valueObj));
        valueObj.value = VariantObj{std::move(valuePtr), variantDef};
    }
}

void Interpreter::operator()(const VarDef& stmt) {
    auto valueRef = getHeldValue(getValueFromExpr(*stmt.expression));
    try {
        valueRef = getHeldValue(convertAndCheckType(stmt.type, std::move(valueRef)));
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{stmt.position, e};
    } catch (const SymbolNotFound& e) {
        throw SymbolNotFound{stmt.position, e};
    } catch (const InvalidFieldCount& e) {
        throw InvalidFieldCount{stmt.position, e};
    }

    try {
        addVariable({.name = std::move(stmt.name),
                     .valueObj = std::make_unique<ValueObj>(std::move(valueRef)),
                     .isConst = stmt.isConst});
    } catch (const VariableRedefinition& e) {
        throw VariableRedefinition{stmt.position, e};
    }
}

struct FieldAccessor {
    explicit FieldAccessor(const Interpreter& interpreter)
        : interpreter_{interpreter} {}

    RefObj operator()(std::string_view name) {
        if (const auto refObj = interpreter_.getVariable(name))
            return *refObj;
        throw SymbolNotFound{{}, "Variable", std::string(name)};
    }

    RefObj operator()(const std::unique_ptr<FieldAccess>& fieldAccess) {
        const auto containerRef = std::visit(*this, fieldAccess->container);
        const auto namedStruct =
            std::get_if<NamedStructObj>(&containerRef.valueObj->value);
        if (!namedStruct)
            throw std::runtime_error("Lhs of field access is not a named struct");
        auto fieldRef = namedStruct->getField(fieldAccess->field);
        return {.valueObj = fieldRef, .isConst = containerRef.isConst};
    }

    const Interpreter& interpreter_;
};

RefObj Interpreter::tryAccessField(const Assignment& stmt) const {
    try {
        return std::visit(FieldAccessor(*this), stmt.lhs);
    } catch (const SymbolNotFound& e) {
        throw SymbolNotFound{stmt.position, e};
    } catch (const InvalidField& e) {
        throw InvalidField{stmt.position, e};
    }
}

void Interpreter::operator()(const Assignment& stmt) {
    auto fieldRef = tryAccessField(stmt);

    if (fieldRef.isConst)
        throw ConstViolation(stmt.position);

    const auto expectedType = std::visit(ValueToType(), fieldRef.valueObj->value);

    auto newValueRef = getValueFromExpr(*stmt.rhs);

    try {
        newValueRef = convertAndCheckType(expectedType, std::move(newValueRef));
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{stmt.position, e};
    } catch (const InvalidFieldCount& e) {
        throw InvalidFieldCount{stmt.position, e};
    }

    fieldRef.valueObj->value = getHeldValue(std::move(newValueRef)).value;
}

void Interpreter::operator()(const FuncDef& stmt) {
    try {
        addFunction(&stmt);
    } catch (const FunctionRedefinition& e) {
        throw FunctionRedefinition{stmt.position, e};
    }
}

void Interpreter::operator()(const FuncCall& funcCall) {
    handleFunctionCall(funcCall);
}

ReturnValue Interpreter::handleFunctionCall(const FuncCall& funcCall) {
    auto funcWithCtx = getFunctionWithCtx(funcCall.name);
    if (!funcWithCtx)
        throw SymbolNotFound{funcCall.position, "Function", funcCall.name};
    auto [funcDef, parentCtx] = *funcWithCtx;

    CallContext ctx{parentCtx};
    passArgumentsToCtx(ctx, funcCall.arguments, funcDef->getParameters());

    static constexpr unsigned int recursionLimit_{1000};
    if (callStack_.size() > recursionLimit_)
        throw MaxRecursionDepth{funcCall.position};

    callStack_.push(std::move(ctx));

    Position lastStmtPosition{funcDef->position};

    for (const auto& stmt : funcDef->getStatements()) {
        stmt->accept(*this);
        if (returning_) {
            lastStmtPosition = stmt->position;
            break;
        }
    }
    returning_ = false;

    if (auto typeName = std::get_if<std::string>(&funcDef->getReturnType()))
        try {
            convertToUserDefinedType(*returnValue_, *typeName);
        } catch (const InvalidFieldCount& e) {
            throw InvalidFieldCount{lastStmtPosition, e};
        } catch (const TypeMismatch& e) {
            throw TypeMismatch{funcDef->position, e};
        } catch (const SymbolNotFound& e) {
            throw SymbolNotFound{funcDef->position, e};
        }

    try {
        checkReturnType(funcDef->getReturnType(), returnValue_);
    } catch (const ReturnTypeMismatch& e) {
        throw ReturnTypeMismatch{lastStmtPosition, e};
    }

    callStack_.pop();

    auto returnValue = std::move(returnValue_);
    returnValue_ = std::nullopt;
    return returnValue;
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

struct VariableAdder {
    VariableAdder(CallContext& callCtx, std::string name)
        : callCtx_{callCtx}, name_{std::move(name)} {}

    void operator()(ValueObj valueObj) const {
        VarEntry varEntry = {.name = name_,
                             .valueObj = std::make_unique<ValueObj>(std::move(valueObj)),
                             .isConst = false};
        callCtx_.addVariable(std::move(varEntry));
    }
    void operator()(RefObj varRef) const {
        RefEntry refEntry = {name_, varRef};
        callCtx_.addReference(std::move(refEntry));
    }

   private:
    CallContext& callCtx_;
    std::string name_;
};

bool isConst(const ValueHolder& holder) {
    if (auto varRef = std::get_if<RefObj>(&holder))
        return varRef->isConst;
    return false;
}

void Interpreter::passArgumentToCtx(CallContext& ctx, const Argument& arg,
                                    const Parameter& param) {
    checkArgRef(arg, param);
    auto valueRef = getValueFromExpr(*arg.value);

    if (!arg.ref)
        valueRef = getHeldValueCopy(std::move(valueRef));

    try {
        valueRef = convertAndCheckType(param.type, std::move(valueRef));
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{arg.position, e};
    } catch (const InvalidFieldCount& e) {
        throw InvalidFieldCount{arg.position, e};
    }

    if (isConst(valueRef))
        throw ConstViolation{arg.position};

    try {
        std::visit(VariableAdder{ctx, param.name}, std::move(valueRef));
    } catch (const VariableRedefinition& e) {
        throw VariableRedefinition{param.position, e};
    }
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
