#include "expr_interpreter.hpp"

#include "interpreter.hpp"
#include "interpreter_errors.hpp"

ExpressionInterpreter::ExpressionInterpreter(const Interpreter& interpreter)
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

void ExpressionInterpreter::operator()(const DisjunctionExpression&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const ConjunctionExpression&) const {
    lastResult_ = nullptr;
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

void ExpressionInterpreter::operator()(const ConversionExpression&) const {
    lastResult_ = nullptr;
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

void ExpressionInterpreter::operator()(const FuncCall&) const {
    lastResult_ = nullptr;
}

void ExpressionInterpreter::operator()(const VariableAccess& expr) const {
    auto variable = interpreter_.getVariable(expr.name);
    if (!variable)
        throw SymbolNotFound{expr.position, "Variable", expr.name};
    lastResult_ = *variable;
}
