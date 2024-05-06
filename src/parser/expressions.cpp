#include "expressions.hpp"

template <typename T>
auto getBinaryExprCtor() {
    return [](Expression lhs, Expression rhs) {
        return std::unique_ptr<T>(new T{std::move(lhs), std::move(rhs)});
    };
}

template <typename T>
auto getUnaryExprCtor() {
    return [](Expression expr) { return std::unique_ptr<T>(new T{std::move(expr)}); };
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
            return [](Expression expr, Type type) {
                return std::unique_ptr<ConversionExpression>(
                    new ConversionExpression{std::move(expr), type});
            };
        case Token::Type::IS_KW:
            return [](Expression expr, Type type) {
                return std::unique_ptr<TypeCheckExpression>(
                    new TypeCheckExpression{std::move(expr), type});
            };
        default:
            return std::nullopt;
    }
}
