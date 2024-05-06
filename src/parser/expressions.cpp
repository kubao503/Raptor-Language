#include "expressions.hpp"

template <typename T>
auto getBinaryExprCtor() {
    return [](Expression lhs, Expression rhs) {
        return std::unique_ptr<T>(new T{std::move(lhs), std::move(rhs)});
    };
}

std::optional<ComparisonExpression::Ctor> ComparisonExpression::getCtor(
    Token::Type type) {
    switch (type) {
        case Token::Type::EQ_OP:
            return getBinaryExprCtor<EqualExpression>();
        case Token::Type::NEQ_OP:
            return getBinaryExprCtor<NotEqualExpression>();
        default:
            return std::nullopt;
    }
}
