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

struct DisjunctionExpression {
    Expression lhs;
    Expression rhs;
};

struct ConjunctionExpression {
    Expression lhs;
    Expression rhs;
};

struct ComparisonExpression {
    Expression lhs;
    Expression rhs;

    using Ctor = std::function<Expression(Expression, Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct EqualExpression : public ComparisonExpression {};

struct NotEqualExpression : public ComparisonExpression {};

struct RelationExpression {
    Expression lhs;
    Expression rhs;

    using Ctor = std::function<Expression(Expression, Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct LessThanExpression : public RelationExpression {};

struct LessThanOrEqualExpression : public RelationExpression {};

struct GreaterThanExpression : public RelationExpression {};

struct GreaterThanOrEqualExpression : public RelationExpression {};

struct AdditiveExpression {
    Expression lhs;
    Expression rhs;

    using Ctor = std::function<Expression(Expression, Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct AdditionExpression : public AdditiveExpression {};

struct SubtractionExpression : public AdditiveExpression {};

struct MultiplicativeExpression {
    Expression lhs;
    Expression rhs;

    using Ctor = std::function<Expression(Expression, Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct MultiplicationExpression : public MultiplicativeExpression {};

struct DivisionExpression : public MultiplicativeExpression {};

struct NegationExpression {
    Expression expr;

    using Ctor = std::function<Expression(Expression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct SignChangeExpression : public NegationExpression {};

struct LogicalNegationExpression : public NegationExpression {};

struct TypeExpression {
    Expression expr;
    Type type;

    using Ctor = std::function<Expression(Expression, Type)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct ConversionExpression : public TypeExpression {};

struct TypeCheckExpression : public TypeExpression {};

struct FieldAccessExpression {
    Expression expr;
    std::string field;
};

struct Argument {
    Expression value;
    bool ref{false};
};

#endif
