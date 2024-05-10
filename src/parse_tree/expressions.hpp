#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "token.hpp"
#include "types.hpp"

enum class BuiltInType {
    INT,
    FLOAT,
    BOOL,
    STR,
};

struct VoidType {};

using Type = std::variant<std::string, BuiltInType>;
using ReturnType = std::variant<std::string, BuiltInType, VoidType>;

struct StructInitExpression;

struct DisjunctionExpression;
struct ConjunctionExpression;
struct EqualExpression;
struct NotEqualExpression;

struct LessThanExpression;
struct LessThanOrEqualExpression;
struct GreaterThanExpression;
struct GreaterThanOrEqualExpression;

struct AdditionExpression;
struct SubtractionExpression;

struct MultiplicationExpression;
struct DivisionExpression;

struct SignChangeExpression;
struct LogicalNegationExpression;

struct ConversionExpression;
struct TypeCheckExpression;

struct FieldAccessExpression;

struct Constant {
    Value value;
};

struct VariableAccess {
    std::string name;
};

struct Argument;
using Arguments = std::vector<Argument>;

struct FuncCall {
    std::string name;
    Arguments arguments;
};

using Expression = std::variant<
    StructInitExpression, std::unique_ptr<DisjunctionExpression>,
    std::unique_ptr<ConjunctionExpression>, std::unique_ptr<EqualExpression>,
    std::unique_ptr<NotEqualExpression>, std::unique_ptr<LessThanExpression>,
    std::unique_ptr<LessThanOrEqualExpression>, std::unique_ptr<GreaterThanExpression>,
    std::unique_ptr<GreaterThanOrEqualExpression>, std::unique_ptr<AdditionExpression>,
    std::unique_ptr<SubtractionExpression>, std::unique_ptr<MultiplicationExpression>,
    std::unique_ptr<DivisionExpression>, std::unique_ptr<SignChangeExpression>,
    std::unique_ptr<LogicalNegationExpression>, std::unique_ptr<ConversionExpression>,
    std::unique_ptr<TypeCheckExpression>, std::unique_ptr<FieldAccessExpression>,
    Constant, VariableAccess, FuncCall>;

struct StructInitExpression {
    std::vector<Expression> exprs;
};

struct BinaryExpression {
    Expression lhs;
    Expression rhs;

    BinaryExpression(Expression lhs, Expression rhs)
        : lhs{std::move(lhs)}, rhs{std::move(rhs)} {}
};

struct DisjunctionExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
};

struct ConjunctionExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
};

struct ComparisonExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<Expression(Expression, Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct EqualExpression : public ComparisonExpression {
    using ComparisonExpression::ComparisonExpression;
};

struct NotEqualExpression : public ComparisonExpression {
    using ComparisonExpression::ComparisonExpression;
};

struct RelationExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<Expression(Expression, Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct LessThanExpression : public RelationExpression {
    using RelationExpression::RelationExpression;
};

struct LessThanOrEqualExpression : public RelationExpression {
    using RelationExpression::RelationExpression;
};

struct GreaterThanExpression : public RelationExpression {
    using RelationExpression::RelationExpression;
};

struct GreaterThanOrEqualExpression : public RelationExpression {
    using RelationExpression::RelationExpression;
};

struct AdditiveExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<Expression(Expression, Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct AdditionExpression : public AdditiveExpression {
    using AdditiveExpression::AdditiveExpression;
};

struct SubtractionExpression : public AdditiveExpression {
    using AdditiveExpression::AdditiveExpression;
};

struct MultiplicativeExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<Expression(Expression, Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct MultiplicationExpression : public MultiplicativeExpression {
    using MultiplicativeExpression::MultiplicativeExpression;
};

struct DivisionExpression : public MultiplicativeExpression {
    using MultiplicativeExpression::MultiplicativeExpression;
};

struct NegationExpression {
    Expression expr;
    NegationExpression(Expression expr)
        : expr{std::move(expr)} {}

    using Ctor = std::function<Expression(Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct SignChangeExpression : public NegationExpression {
    using NegationExpression::NegationExpression;
};

struct LogicalNegationExpression : public NegationExpression {
    using NegationExpression::NegationExpression;
};

struct TypeExpression {
    Expression expr;
    Type type;
    TypeExpression(Expression expr, Type type)
        : expr{std::move(expr)}, type{std::move(type)} {}

    using Ctor = std::function<Expression(Expression, Type)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct ConversionExpression : public TypeExpression {
    using TypeExpression::TypeExpression;
};

struct TypeCheckExpression : public TypeExpression {
    using TypeExpression::TypeExpression;
};

struct FieldAccessExpression {
    Expression expr;
    std::string field;
};

struct Argument {
    Expression value;
    bool ref{false};
};

#endif
