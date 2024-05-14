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

struct Constant;
struct FuncCall;
struct VariableAccess;

class ExpressionVisitor {
   public:
    virtual void operator()(const StructInitExpression& expr) const = 0;
    virtual void operator()(const DisjunctionExpression& disjunction) const = 0;
    virtual void operator()(const ConjunctionExpression& conjunction) const = 0;
    virtual void operator()(const EqualExpression& expr) const = 0;
    virtual void operator()(const NotEqualExpression& expr) const = 0;
    virtual void operator()(const LessThanExpression& expr) const = 0;
    virtual void operator()(const LessThanOrEqualExpression& expr) const = 0;
    virtual void operator()(const GreaterThanExpression& expr) const = 0;
    virtual void operator()(const GreaterThanOrEqualExpression& expr) const = 0;
    virtual void operator()(const AdditionExpression& expr) const = 0;
    virtual void operator()(const SubtractionExpression& expr) const = 0;
    virtual void operator()(const MultiplicationExpression& expr) const = 0;
    virtual void operator()(const DivisionExpression& expr) const = 0;
    virtual void operator()(const SignChangeExpression& expr) const = 0;
    virtual void operator()(const LogicalNegationExpression& expr) const = 0;
    virtual void operator()(const ConversionExpression& expr) const = 0;
    virtual void operator()(const TypeCheckExpression& expr) const = 0;
    virtual void operator()(const FieldAccessExpression& expr) const = 0;
    virtual void operator()(const Constant& expr) const = 0;
    virtual void operator()(const FuncCall& expr) const = 0;
    virtual void operator()(const VariableAccess& expr) const = 0;
};

class Expression {
   public:
    virtual ~Expression() = default;
    virtual void accept(const ExpressionVisitor& vis) const = 0;
};

using PExpression = std::unique_ptr<Expression>;

struct StructInitExpression : public Expression {
    std::vector<PExpression> exprs;

    StructInitExpression(std::vector<PExpression> exprs)
        : exprs{std::move(exprs)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct BinaryExpression : public Expression {
    PExpression lhs;
    PExpression rhs;

    BinaryExpression(PExpression lhs, PExpression rhs)
        : lhs{std::move(lhs)}, rhs{std::move(rhs)} {}
};

struct DisjunctionExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct ConjunctionExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct ComparisonExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct EqualExpression : public ComparisonExpression {
    using ComparisonExpression::ComparisonExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct NotEqualExpression : public ComparisonExpression {
    using ComparisonExpression::ComparisonExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct RelationExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct LessThanExpression : public RelationExpression {
    using RelationExpression::RelationExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct LessThanOrEqualExpression : public RelationExpression {
    using RelationExpression::RelationExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct GreaterThanExpression : public RelationExpression {
    using RelationExpression::RelationExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct GreaterThanOrEqualExpression : public RelationExpression {
    using RelationExpression::RelationExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct AdditiveExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct AdditionExpression : public AdditiveExpression {
    using AdditiveExpression::AdditiveExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct SubtractionExpression : public AdditiveExpression {
    using AdditiveExpression::AdditiveExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct MultiplicativeExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct MultiplicationExpression : public MultiplicativeExpression {
    using MultiplicativeExpression::MultiplicativeExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct DivisionExpression : public MultiplicativeExpression {
    using MultiplicativeExpression::MultiplicativeExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct NegationExpression : public Expression {
    PExpression expr;
    NegationExpression(PExpression expr)
        : expr{std::move(expr)} {}

    using Ctor = std::function<PExpression(PExpression)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct SignChangeExpression : public NegationExpression {
    using NegationExpression::NegationExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct LogicalNegationExpression : public NegationExpression {
    using NegationExpression::NegationExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct TypeExpression : public Expression {
    PExpression expr;
    Type type;
    TypeExpression(PExpression expr, Type type)
        : expr{std::move(expr)}, type{std::move(type)} {}

    using Ctor = std::function<PExpression(PExpression, Type)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct ConversionExpression : public TypeExpression {
    using TypeExpression::TypeExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct TypeCheckExpression : public TypeExpression {
    using TypeExpression::TypeExpression;

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct FieldAccessExpression : public Expression {
    PExpression expr;
    std::string field;

    FieldAccessExpression(PExpression expr, std::string field)
        : expr{std::move(expr)}, field{std::move(field)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct Constant : public Expression {
    Value value;

    Constant(Value value)
        : value{std::move(value)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct Argument {
    PExpression value;
    bool ref{false};
};

using Arguments = std::vector<Argument>;

struct FuncCall : public Expression {
    std::string name;
    Arguments arguments;

    FuncCall(std::string name, Arguments arguments)
        : name{std::move(name)}, arguments{std::move(arguments)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct VariableAccess : public Expression {
    std::string name;

    VariableAccess(std::string name)
        : name{std::move(name)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

#endif
