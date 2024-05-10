#include "expressions.hpp"

template <typename T>
auto getBinaryExprCtor() {
    return [](auto lhs, auto rhs) {
        return std::make_unique<T>(std::move(lhs), std::move(rhs));
    };
}

template <typename T>
auto getUnaryExprCtor() {
    return [](auto expr) { return std::make_unique<T>(std::move(expr)); };
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

std::optional<RelationExpression::Ctor> RelationExpression::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::LT_OP:
            return getBinaryExprCtor<LessThanExpression>();
        case Token::Type::LTE_OP:
            return getBinaryExprCtor<LessThanOrEqualExpression>();
        case Token::Type::GT_OP:
            return getBinaryExprCtor<GreaterThanExpression>();
        case Token::Type::GTE_OP:
            return getBinaryExprCtor<GreaterThanOrEqualExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<AdditiveExpression::Ctor> AdditiveExpression::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::ADD_OP:
            return getBinaryExprCtor<AdditionExpression>();
        case Token::Type::MIN_OP:
            return getBinaryExprCtor<SubtractionExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<MultiplicativeExpression::Ctor> MultiplicativeExpression::getCtor(
    Token::Type type) {
    switch (type) {
        case Token::Type::MULT_OP:
            return getBinaryExprCtor<MultiplicationExpression>();
        case Token::Type::DIV_OP:
            return getBinaryExprCtor<DivisionExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<NegationExpression::Ctor> NegationExpression::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::MIN_OP:
            return getUnaryExprCtor<SignChangeExpression>();
        case Token::Type::NOT_KW:
            return getUnaryExprCtor<LogicalNegationExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<TypeExpression::Ctor> TypeExpression::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::AS_KW:
            return getBinaryExprCtor<ConversionExpression>();
        case Token::Type::IS_KW:
            return getBinaryExprCtor<TypeCheckExpression>();
        default:
            return std::nullopt;
    }
}
