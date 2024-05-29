#include "expr_interpreter.hpp"

#include <algorithm>

#include "interpreter.hpp"
#include "interpreter_errors.hpp"

ExpressionInterpreter::ExpressionInterpreter(Interpreter* interpreter)
    : interpreter_{interpreter} {
    if (!interpreter_)
        throw std::runtime_error("Null interpreter pointer");
}

void ExpressionInterpreter::operator()(const StructInitExpression& expr) const {
    auto exprToStructValue = [this](const PExpression& expr) {
        auto value = getExprValue(*expr);
        return std::make_unique<ValueObj>(std::move(value));
    };
    StructObj structObj;
    std::ranges::transform(expr.exprs, std::back_inserter(structObj.values),
                           exprToStructValue);
    lastResult_ = ValueObj{std::move(structObj)};
}

ValueObj ExpressionInterpreter::getExprValue(const Expression& expr) const {
    expr.accept(*this);
    return getHeldValue(std::move(lastResult_));
}

bool ExpressionInterpreter::getBoolValue(const Expression& expr) const {
    const auto valueObj = getExprValue(expr);
    const auto boolValue = std::get_if<bool>(&valueObj.value);
    if (!boolValue)
        throw TypeMismatch{expr.position, BuiltInType::BOOL,
                           std::visit(ValueToType(), valueObj.value)};

    return *boolValue;
}

void ExpressionInterpreter::operator()(const DisjunctionExpression& expr) const {
    auto leftBool = getBoolValue(*expr.lhs);
    auto rightBool = getBoolValue(*expr.rhs);
    auto result = leftBool || rightBool;
    lastResult_ = ValueObj{result};
}

void ExpressionInterpreter::operator()(const ConjunctionExpression& expr) const {
    auto leftBool = getBoolValue(*expr.lhs);
    auto rightBool = getBoolValue(*expr.rhs);
    auto result = leftBool && rightBool;
    lastResult_ = ValueObj{result};
}

struct EqualityEvaluator {
    bool operator()(bool lhs, bool rhs) const { return lhs == rhs; }
    bool operator()(Integral lhs, Integral rhs) const { return lhs == rhs; }
    bool operator()(Floating lhs, Floating rhs) const { return lhs == rhs; }
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        return lhs == rhs;
    }
    bool operator()(const auto& lhs, const auto& rhs) const {
        throw TypeMismatch{{}, ValueToType()(lhs), ValueToType()(rhs)};
    }
};

void ExpressionInterpreter::checkExprEquality(const BinaryExpression& expr) const {
    expr.lhs->accept(*this);
    const auto leftValueObj = getHeldValue(std::move(lastResult_));
    expr.rhs->accept(*this);
    const auto rightValueObj = getHeldValue(std::move(lastResult_));

    try {
        const auto result =
            std::visit(EqualityEvaluator(), leftValueObj.value, rightValueObj.value);
        lastResult_ = ValueObj{result};
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{expr.position, e};
    }
}

void ExpressionInterpreter::operator()(const EqualExpression& expr) const {
    checkExprEquality(expr);
}

void ExpressionInterpreter::operator()(const NotEqualExpression& expr) const {
    checkExprEquality(expr);
    const auto valueObj = getHeldValue(std::move(lastResult_));
    const auto boolValue = std::get<bool>(valueObj.value);
    lastResult_ = ValueObj{!boolValue};
}

template <typename Functor>
struct ComparisonEvaluator {
    ComparisonEvaluator(Functor func)
        : func_{func} {}

    bool operator()(Integral lhs, Integral rhs) { return func_(lhs, rhs); }
    bool operator()(Floating lhs, Floating rhs) { return func_(lhs, rhs); }
    bool operator()(const std::string& lhs, const std::string& rhs) {
        return func_(lhs, rhs);
    }
    bool operator()(const auto& lhs, const auto& rhs) {
        throw TypeMismatch{{}, ValueToType()(lhs), ValueToType()(rhs)};
    }

    Functor func_;
};

template <typename Functor>
void ExpressionInterpreter::compareExpr(const BinaryExpression& expr,
                                        Functor func) const {
    expr.lhs->accept(*this);
    const auto leftValue = getHeldValue(std::move(lastResult_)).value;
    expr.rhs->accept(*this);
    const auto rightValue = getHeldValue(std::move(lastResult_)).value;

    try {
        const auto result = std::visit(ComparisonEvaluator(func), leftValue, rightValue);
        lastResult_ = ValueObj{result};
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{expr.position, e};
    }
}

void ExpressionInterpreter::operator()(const LessThanExpression& expr) const {
    compareExpr(expr, std::less());
}

void ExpressionInterpreter::operator()(const LessThanOrEqualExpression& expr) const {
    compareExpr(expr, std::less_equal());
}

void ExpressionInterpreter::operator()(const GreaterThanExpression& expr) const {
    compareExpr(expr, std::greater());
}

void ExpressionInterpreter::operator()(const GreaterThanOrEqualExpression& expr) const {
    compareExpr(expr, std::greater_equal());
}

template <typename Functor>
struct NumericEvaluator {
    NumericEvaluator(Functor func)
        : func_{func} {}

    ValueObj::Value operator()(Integral lhs, Integral rhs) const {
        return func_(lhs, rhs);
    }
    ValueObj::Value operator()(Floating lhs, Floating rhs) const {
        return func_(lhs, rhs);
    }
    ValueObj::Value operator()(const auto& lhs, const auto& rhs) const {
        throw TypeMismatch{{}, ValueToType()(lhs), ValueToType()(rhs)};
    }

    Functor func_;
};

template <>
struct NumericEvaluator<std::divides<>> {
    NumericEvaluator(std::divides<>) {}

    ValueObj::Value operator()(Integral lhs, Integral rhs) const {
        if (rhs == 0)
            throw DivisionByZero{{}};
        return lhs / rhs;
    }
    ValueObj::Value operator()(Floating lhs, Floating rhs) const {
        if (rhs == 0.0f)
            throw DivisionByZero{{}};
        return lhs / rhs;
    }
    ValueObj::Value operator()(const auto& lhs, const auto& rhs) const {
        throw TypeMismatch{{}, ValueToType()(lhs), ValueToType()(rhs)};
    }
};

template <typename Functor>
void ExpressionInterpreter::evalNumericExpr(const BinaryExpression& expr,
                                            Functor func) const {
    const auto leftValueObj = getExprValue(*expr.lhs);
    const auto rightValueObj = getExprValue(*expr.rhs);

    try {
        auto value =
            std::visit(NumericEvaluator(func), leftValueObj.value, rightValueObj.value);
        lastResult_ = ValueObj{std::move(value)};
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{expr.position, e};
    } catch (const DivisionByZero&) {
        throw DivisionByZero{expr.position};
    }
}

struct AdditionEvaluator {
    ValueObj::Value operator()(const std::string& lhs, const std::string& rhs) const {
        return lhs + rhs;
    }
    ValueObj::Value operator()(const auto& lhs, const auto& rhs) const {
        return NumericEvaluator(std::plus())(lhs, rhs);
    }
};

void ExpressionInterpreter::operator()(const AdditionExpression& expr) const {
    const auto leftValue = getExprValue(*expr.lhs);
    const auto rightValue = getExprValue(*expr.rhs);

    try {
        auto value = std::visit(AdditionEvaluator(), leftValue.value, rightValue.value);
        lastResult_ = ValueObj{std::move(value)};
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{expr.position, e};
    }
}

void ExpressionInterpreter::operator()(const SubtractionExpression& expr) const {
    evalNumericExpr(expr, std::minus());
}

void ExpressionInterpreter::operator()(const MultiplicationExpression& expr) const {
    evalNumericExpr(expr, std::multiplies());
}

void ExpressionInterpreter::operator()(const DivisionExpression& expr) const {
    evalNumericExpr(expr, std::divides());
}

struct SignChanger {
    ValueObj::Value operator()(Integral i) { return -i; }
    ValueObj::Value operator()(Floating i) { return -i; }
    ValueObj::Value operator()(const auto& i) {
        throw TypeMismatch{{}, BuiltInType::INT, ValueToType()(i)};
    }
};

void ExpressionInterpreter::operator()(const SignChangeExpression& expr) const {
    const auto value = getExprValue(*expr.expr);

    try {
        auto result = std::visit(SignChanger(), value.value);
        lastResult_ = ValueObj{std::move(result)};
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{expr.position, e};
    }
}

void ExpressionInterpreter::operator()(const LogicalNegationExpression& expr) const {
    const auto value = getBoolValue(*expr.expr);
    lastResult_ = ValueObj{!value};
}

struct TypeConverter {
    explicit TypeConverter(Interpreter* interpreter)
        : interpreter_{interpreter} {
        if (!interpreter_)
            throw std::runtime_error("Null interpreter pointer");
    }

    ValueObj::Value operator()(VariantObj from, const auto& to) const {
        if (std::visit(TypeComparer(), static_cast<Type>(to), from.valueObj->value))
            return std::move(from.valueObj->value);

        throw InvalidTypeConversion{{}, std::move(from.valueObj->value), to};
    }
    ValueObj::Value operator()(Integral from, BuiltInType to) const {
        return fromBuiltInValue(from, to);
    }
    ValueObj::Value operator()(Floating from, BuiltInType to) const {
        return fromBuiltInValue(from, to);
    }
    ValueObj::Value operator()(bool from, BuiltInType to) const {
        return fromBuiltInValue(from, to);
    }
    ValueObj::Value operator()(std::string from, BuiltInType to) const {
        if (to == BuiltInType::STR)
            return from;
        throw InvalidTypeConversion{{}, std::move(from), to};
    }
    ValueObj::Value operator()(NamedStructObj from, const std::string& to) const {
        if (from.structDef->name == to)
            return from;
        return convertToVariant(std::move(from), to);
    }

    ValueObj::Value operator()(Integral from, const std::string& to) const {
        return convertToVariant(from, to);
    }
    ValueObj::Value operator()(Floating from, const std::string& to) const {
        return convertToVariant(from, to);
    }
    ValueObj::Value operator()(bool from, const std::string& to) const {
        return convertToVariant(from, to);
    }
    ValueObj::Value operator()(std::string from, const std::string& to) const {
        return convertToVariant(std::move(from), to);
    }

    ValueObj::Value operator()(auto from, const auto& to) const {
        throw InvalidTypeConversion{{}, std::move(from), to};
    }

   private:
    ValueObj::Value fromBuiltInValue(auto builtInValue, BuiltInType to) const {
        switch (to) {
            case BuiltInType::INT:
                return static_cast<Integral>(builtInValue);
            case BuiltInType::FLOAT:
                return static_cast<Floating>(builtInValue);
            case BuiltInType::BOOL:
                return static_cast<bool>(builtInValue);
            default:
                throw InvalidTypeConversion{{}, builtInValue, to};
        }
    }

    ValueObj::Value convertToVariant(auto from, const std::string& to) const {
        const auto variantDef = interpreter_->getVariantDef(to);
        if (!variantDef)
            throw InvalidTypeConversion{{}, std::move(from), to};

        auto value = static_cast<ValueObj::Value>(std::move(from));

        auto compareTypeWithValue = [&](const Type& type) {
            return std::visit(TypeComparer(), type, value);
        };
        if (std::ranges::any_of(variantDef->types, compareTypeWithValue)) {
            auto valuePtr = std::make_unique<ValueObj>(std::move(value));
            return VariantObj{std::move(valuePtr), variantDef};
        }
        throw InvalidTypeConversion{{}, std::move(value), to};
    }

    Interpreter* interpreter_;
};

void ExpressionInterpreter::operator()(const ConversionExpression& conversionExpr) const {
    auto valueObj = getExprValue(*conversionExpr.expr);

    try {
        auto value = std::visit(TypeConverter(interpreter_), std::move(valueObj.value),
                                conversionExpr.type);
        lastResult_ = ValueObj{std::move(value)};
    } catch (InvalidTypeConversion& e) {
        throw InvalidTypeConversion{conversionExpr.position, std::move(e)};
    }
}

struct TypeChecker {
    TypeChecker(Type expected)
        : expected_{expected} {}

    bool operator()(const VariantObj& variantObj) const {
        return std::visit(TypeComparer(), expected_, variantObj.valueObj->value);
    }
    bool operator()(const auto& value) const { return TypeComparer()(expected_, value); }

    Type expected_;
};

void ExpressionInterpreter::operator()(const TypeCheckExpression& expr) const {
    const auto valueObj = getExprValue(*expr.expr);

    const auto result = std::visit(TypeChecker(expr.type), valueObj.value);
    lastResult_ = ValueObj{result};
}

struct FieldAccessor {
    explicit FieldAccessor(const FieldAccessExpression& expr)
        : expr_{expr} {}

    ValueHolder operator()(const RefObj& ref) const {
        const auto valueObj = ref.valueObj;
        const auto fieldRef = tryGetNamedStructField(*valueObj);

        return RefObj{.valueObj = fieldRef, .isConst = ref.isConst};
    }

    ValueHolder operator()(const ValueObj& valueObj) const {
        const auto fieldRef = tryGetNamedStructField(valueObj);
        return ValueObj{std::move(fieldRef->value)};
    }

   private:
    ValueObj* tryGetNamedStructField(const ValueObj& valueObj) const {
        const auto namedStructObj = std::get_if<NamedStructObj>(&valueObj.value);
        if (!namedStructObj)
            throw TypeMismatch{expr_.position, "Named struct",
                               std::visit(ValueToType(), valueObj.value)};

        try {
            return namedStructObj->getField(expr_.field);
        } catch (const InvalidField& e) {
            throw InvalidField{expr_.position, e};
        }
    }

    const FieldAccessExpression& expr_;
};

void ExpressionInterpreter::operator()(const FieldAccessExpression& expr) const {
    expr.expr->accept(*this);
    auto fieldValue = std::visit(FieldAccessor(expr), lastResult_);
    lastResult_ = std::move(fieldValue);
}

void ExpressionInterpreter::operator()(const Constant& expr) const {
    auto value =
        std::visit([](const auto& v) -> ValueObj::Value { return v; }, expr.value);
    lastResult_ = ValueObj{std::move(value)};
}

void ExpressionInterpreter::operator()(const FuncCall& funcCall) const {
    auto value = interpreter_->handleFunctionCall(funcCall);
    if (!value)
        throw TypeMismatch{funcCall.position, "NON-VOID", "VOID"};
    lastResult_ = std::move(*value);
}

void ExpressionInterpreter::operator()(const VariableAccess& expr) const {
    auto varRef = interpreter_->getVariable(expr.name);
    if (!varRef)
        throw SymbolNotFound{expr.position, "Variable", expr.name};
    lastResult_ = *varRef;
}
