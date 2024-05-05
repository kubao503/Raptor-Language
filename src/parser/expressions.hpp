#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <memory>
#include <string>
#include <variant>
#include <vector>

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
struct NegationExpression;

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
    std::unique_ptr<NegationExpression>, std::unique_ptr<ConversionExpression>,
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

struct EqualExpression {
    Expression lhs;
    Expression rhs;
};

struct NotEqualExpression {
    Expression lhs;
    Expression rhs;
};

struct LessThanExpression {
    Expression lhs;
    Expression rhs;
};

struct LessThanOrEqualExpression {
    Expression lhs;
    Expression rhs;
};

struct GreaterThanExpression {
    Expression lhs;
    Expression rhs;
};

struct GreaterThanOrEqualExpression {
    Expression lhs;
    Expression rhs;
};

struct AdditionExpression {
    Expression lhs;
    Expression rhs;
};

struct SubtractionExpression {
    Expression lhs;
    Expression rhs;
};

struct MultiplicationExpression {
    Expression lhs;
    Expression rhs;
};

struct DivisionExpression {
    Expression lhs;
    Expression rhs;
};

struct SignChangeExpression {
    Expression expr;
};

struct NegationExpression {
    Expression expr;
};

struct ConversionExpression {
    Expression expr;
    Type type;
};

struct TypeCheckExpression {
    Expression expr;
    Type type;
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
