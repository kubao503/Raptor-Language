#include "expressions.hpp"

template <typename T>
auto getCtorObject() {
    return [](auto&&... args) {
        return std::make_unique<T>(std::forward<decltype(args)>(args)...);
    };
}

std::optional<ComparisonExpression::Ctor> ComparisonExpression::getCtor(
    Token::Type type) {
    switch (type) {
        case Token::Type::EQ_OP:
            return getCtorObject<EqualExpression>();
        case Token::Type::NEQ_OP:
            return getCtorObject<NotEqualExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<RelationExpression::Ctor> RelationExpression::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::LT_OP:
            return getCtorObject<LessThanExpression>();
        case Token::Type::LTE_OP:
            return getCtorObject<LessThanOrEqualExpression>();
        case Token::Type::GT_OP:
            return getCtorObject<GreaterThanExpression>();
        case Token::Type::GTE_OP:
            return getCtorObject<GreaterThanOrEqualExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<AdditiveExpression::Ctor> AdditiveExpression::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::ADD_OP:
            return getCtorObject<AdditionExpression>();
        case Token::Type::MIN_OP:
            return getCtorObject<SubtractionExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<MultiplicativeExpression::Ctor> MultiplicativeExpression::getCtor(
    Token::Type type) {
    switch (type) {
        case Token::Type::MULT_OP:
            return getCtorObject<MultiplicationExpression>();
        case Token::Type::DIV_OP:
            return getCtorObject<DivisionExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<NegationExpression::Ctor> NegationExpression::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::MIN_OP:
            return getCtorObject<SignChangeExpression>();
        case Token::Type::NOT_KW:
            return getCtorObject<LogicalNegationExpression>();
        default:
            return std::nullopt;
    }
}

std::optional<TypeExpression::Ctor> TypeExpression::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::AS_KW:
            return getCtorObject<ConversionExpression>();
        case Token::Type::IS_KW:
            return getCtorObject<TypeCheckExpression>();
        default:
            return std::nullopt;
    }
}
