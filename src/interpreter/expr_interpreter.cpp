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

void ExpressionInterpreter::operator()(const EqualExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const NotEqualExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const LessThanExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const LessThanOrEqualExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const GreaterThanExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const GreaterThanOrEqualExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const AdditionExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const SubtractionExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const MultiplicationExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const DivisionExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const SignChangeExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const LogicalNegationExpression&) const {
    lastResult_ = nullptr;
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
        auto value = std::visit(TypeConverter(), lastResult_->value, conversionExpr.type);
        lastResult_ = std::make_shared<ValueObj>(value);
    } catch (const InvalidTypeConversion& e) {
        throw InvalidTypeConversion{conversionExpr.position, e};
    }
}

void ExpressionInterpreter::operator()(const TypeCheckExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const FieldAccessExpression& expr) const {
    expr.expr->accept(*this);
    auto namedStructObj = std::get_if<NamedStructObj>(&(lastResult_->value));
    if (!namedStructObj)
        throw std::runtime_error(
            "Cannot access a field of expression which is not a structure");
    auto structDef = namedStructObj->structDef;
    auto res = std::ranges::find(structDef->fields, expr.field, &Field::name);
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
    auto variable = interpreter_.getVariable(expr.name);
    if (!variable)
        throw SymbolNotFound{expr.position, "Variable", expr.name};
    lastResult_ = *variable;
}
