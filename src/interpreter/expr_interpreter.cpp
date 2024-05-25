#include "expr_interpreter.hpp"

#include "interpreter.hpp"
#include "interpreter_errors.hpp"

ExpressionInterpreter::ExpressionInterpreter(Interpreter& interpreter)
    : interpreter_{interpreter} {}

void ExpressionInterpreter::operator()(const StructInitExpression& expr) const {
    StructObj structObj;
    auto exprToValueRef = [this](const PExpression& expr) {
        expr->accept(*this);
        return this->lastResult_;
    };
    std::ranges::transform(expr.exprs, std::back_inserter(structObj.values),
                           exprToValueRef);
    lastResult_ = std::make_shared<ValueObj>(std::move(structObj));
}

std::pair<bool, bool> ExpressionInterpreter::getBoolExpression(Expression* lhs,
                                                               Expression* rhs) const {
    lhs->accept(*this);
    auto leftValue = std::get_if<bool>(&lastResult_->value);
    if (!leftValue)
        throw TypeMismatch{lhs->position, BuiltInType::BOOL,
                           std::visit(ValueToType(), lastResult_->value)};

    rhs->accept(*this);
    auto rightValue = std::get_if<bool>(&lastResult_->value);
    if (!rightValue)
        throw TypeMismatch{rhs->position, BuiltInType::BOOL,
                           std::visit(ValueToType(), lastResult_->value)};
    return {*leftValue, *rightValue};
}

void ExpressionInterpreter::operator()(const DisjunctionExpression& expr) const {
    auto boolPair = getBoolExpression(expr.lhs.get(), expr.rhs.get());
    auto result = boolPair.first || boolPair.second;
    lastResult_ = std::make_shared<ValueObj>(result);
}

void ExpressionInterpreter::operator()(const ConjunctionExpression& expr) const {
    auto boolPair = getBoolExpression(expr.lhs.get(), expr.rhs.get());
    auto result = boolPair.first && boolPair.second;
    lastResult_ = std::make_shared<ValueObj>(result);
}

struct EqualityEvaluator {
    bool operator()(bool lhs, bool rhs) const { return lhs == rhs; }
    bool operator()(Integral lhs, Integral rhs) const { return lhs == rhs; }
    bool operator()(Floating lhs, Floating rhs) const { return lhs == rhs; }
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        return lhs == rhs;
    }
    bool operator()(const auto& lhs, const auto& rhs) const {
        throw TypeMismatch{{},
                           std::visit(ValueToType(), static_cast<ValueObj::Value>(lhs)),
                           std::visit(ValueToType(), static_cast<ValueObj::Value>(rhs))};
    }
};

void ExpressionInterpreter::checkExprEquality(const BinaryExpression& expr) const {
    expr.lhs->accept(*this);
    const auto leftValue = lastResult_->value;
    expr.rhs->accept(*this);
    const auto rightValue = lastResult_->value;

    const auto result = std::visit(EqualityEvaluator(), leftValue, rightValue);
    lastResult_ = std::make_shared<ValueObj>(result);
}

void ExpressionInterpreter::operator()(const EqualExpression& expr) const {
    checkExprEquality(expr);
}

void ExpressionInterpreter::operator()(const NotEqualExpression& expr) const {
    checkExprEquality(expr);
    const auto value = std::get<bool>(lastResult_->value);
    lastResult_ = std::make_shared<ValueObj>(!value);
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
        throw TypeMismatch{{},
                           std::visit(ValueToType(), static_cast<ValueObj::Value>(lhs)),
                           std::visit(ValueToType(), static_cast<ValueObj::Value>(rhs))};
    }

    Functor func_;
};

template <typename Functor>
void ExpressionInterpreter::compareExpr(const BinaryExpression& expr,
                                        Functor func) const {
    expr.lhs->accept(*this);
    const auto leftValue = lastResult_->value;
    expr.rhs->accept(*this);
    const auto rightValue = lastResult_->value;

    try {
        const auto result = std::visit(ComparisonEvaluator(func), leftValue, rightValue);
        lastResult_ = std::make_shared<ValueObj>(result);
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
        throw TypeMismatch{{},
                           std::visit(ValueToType(), static_cast<ValueObj::Value>(lhs)),
                           std::visit(ValueToType(), static_cast<ValueObj::Value>(rhs))};
    }

    Functor func_;
};

template <typename Functor>
void ExpressionInterpreter::evalNumericExpr(const BinaryExpression& expr,
                                            Functor func) const {
    expr.lhs->accept(*this);
    const auto leftValue = lastResult_->value;
    expr.rhs->accept(*this);
    const auto rightValue = lastResult_->value;

    try {
        const auto value = std::visit(NumericEvaluator(func), leftValue, rightValue);
        lastResult_ = std::make_shared<ValueObj>(value);
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{expr.position, e};
    }
}

struct AdditionEvaluator {
    ValueObj::Value operator()(const std::string& lhs, const std::string& rhs) const {
        return lhs + rhs;
    }
    ValueObj::Value operator()(const auto& lhs, const auto& rhs) const {
        return std::visit(NumericEvaluator(std::plus()),
                          static_cast<ValueObj::Value>(lhs),
                          static_cast<ValueObj::Value>(rhs));
    }
};

void ExpressionInterpreter::operator()(const AdditionExpression& expr) const {
    expr.lhs->accept(*this);
    const auto leftValue = lastResult_->value;
    expr.rhs->accept(*this);
    const auto rightValue = lastResult_->value;

    try {
        const auto value = std::visit(AdditionEvaluator(), leftValue, rightValue);
        lastResult_ = std::make_shared<ValueObj>(value);
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
        throw TypeMismatch{{},
                           BuiltInType::INT,
                           std::visit(ValueToType(), static_cast<ValueObj::Value>(i))};
    }
};

void ExpressionInterpreter::operator()(const SignChangeExpression& expr) const {
    expr.expr->accept(*this);
    const auto value = lastResult_->value;

    try {
        auto result = std::visit(SignChanger(), value);
        lastResult_ = std::make_shared<ValueObj>(result);
    } catch (const TypeMismatch& e) {
        throw TypeMismatch{expr.position, e};
    }
}

void ExpressionInterpreter::operator()(const LogicalNegationExpression& expr) const {
    expr.expr->accept(*this);

    const auto value = std::get_if<bool>(&lastResult_->value);
    if (!value)
        throw TypeMismatch{expr.position, BuiltInType::BOOL,
                           std::visit(ValueToType(), lastResult_->value)};
    lastResult_ = std::make_shared<ValueObj>(!(*value));
}

struct TypeConverter {
    ValueObj::Value operator()(const VariantObj& from, const auto& to) const {
        if (std::visit(TypeComparer(), static_cast<Type>(to), from.valueRef->value))
            return from.valueRef->value;
        throw InvalidTypeConversion{{}, from, to};
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
    ValueObj::Value operator()(const std::string& from, BuiltInType to) const {
        if (to == BuiltInType::STR)
            return from;
        throw InvalidTypeConversion{{}, from, to};
    }
    ValueObj::Value operator()(const NamedStructObj& from, const std::string& to) const {
        if (from.structDef->name == to)
            return from;
        throw InvalidTypeConversion{{}, from, to};
    }
    ValueObj::Value operator()(auto from, auto to) const {
        throw InvalidTypeConversion{{}, from, to};
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
};

void ExpressionInterpreter::operator()(const ConversionExpression& conversionExpr) const {
    conversionExpr.expr->accept(*this);

    try {
        const auto value =
            std::visit(TypeConverter(), lastResult_->value, conversionExpr.type);
        lastResult_ = std::make_shared<ValueObj>(value);
    } catch (const InvalidTypeConversion& e) {
        throw InvalidTypeConversion{conversionExpr.position, e};
    }
}

struct TypeChecker {
    TypeChecker(Type expected)
        : expected_{expected} {}

    bool operator()(const VariantObj& value) const {
        return std::visit(TypeComparer(), expected_, value.valueRef->value);
    }
    bool operator()(const auto& value) const {
        return std::visit(TypeComparer(), expected_, static_cast<ValueObj::Value>(value));
    }

    Type expected_;
};

void ExpressionInterpreter::operator()(const TypeCheckExpression& expr) const {
    expr.expr->accept(*this);
    const auto value = lastResult_->value;

    const auto result = std::visit(TypeChecker(expr.type), value);
    lastResult_ = std::make_shared<ValueObj>(result);
}

void ExpressionInterpreter::operator()(const FieldAccessExpression& expr) const {
    expr.expr->accept(*this);
    auto namedStructObj = std::get_if<NamedStructObj>(&(lastResult_->value));
    if (!namedStructObj)
        throw TypeMismatch{expr.position, "Named struct",
                           std::visit(ValueToType(), lastResult_->value)};
    auto structDef = namedStructObj->structDef;
    auto res = std::ranges::find(structDef->fields, expr.field, &Field::name);
    if (res == structDef->fields.end())
        throw InvalidField{expr.position, expr.field};
    auto index = std::distance(structDef->fields.begin(), res);
    lastResult_ = namedStructObj->values.at(index);
}

void ExpressionInterpreter::operator()(const Constant& expr) const {
    auto value =
        std::visit([](const auto& v) -> ValueObj::Value { return v; }, expr.value);
    lastResult_ = std::make_shared<ValueObj>(std::move(value));
}

void ExpressionInterpreter::operator()(const FuncCall& funcCall) const {
    interpreter_(funcCall);
    auto value = interpreter_.handleFunctionCall(funcCall);
    lastResult_ = std::make_shared<ValueObj>(*value);
}

void ExpressionInterpreter::operator()(const VariableAccess& expr) const {
    auto varEntry = interpreter_.getVariable(expr.name);
    if (!varEntry)
        throw SymbolNotFound{expr.position, "Variable", expr.name};
    lastResult_ = varEntry->valueRef;
}
