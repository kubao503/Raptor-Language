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
#include "visitor.hpp"

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

struct SyntaxNode {
    virtual ~SyntaxNode() = default;

    SyntaxNode(const Position& position)
        : position{position} {}

    Position position;
};

using ExpressionVisitor =
    ConstVisitor<StructInitExpression, DisjunctionExpression, ConjunctionExpression,
                 EqualExpression, NotEqualExpression, LessThanExpression,
                 LessThanOrEqualExpression, GreaterThanExpression,
                 GreaterThanOrEqualExpression, AdditionExpression, SubtractionExpression,
                 MultiplicationExpression, DivisionExpression, SignChangeExpression,
                 LogicalNegationExpression, ConversionExpression, TypeCheckExpression,
                 FieldAccessExpression, Constant, FuncCall, VariableAccess>;

struct Expression : public virtual SyntaxNode,
                    public virtual ConstVisitable<ExpressionVisitor> {};

using PExpression = std::unique_ptr<Expression>;

template <typename T>
using VisitableExpression = ConstVisitableImpl<T, ExpressionVisitor>;

struct StructInitExpression : public Expression,
                              public VisitableExpression<StructInitExpression> {
    std::vector<PExpression> exprs;

    StructInitExpression(std::vector<PExpression> exprs, const Position& position)
        : SyntaxNode{position}, exprs{std::move(exprs)} {}
};

struct BinaryExpression : public Expression {
    PExpression lhs;
    PExpression rhs;

    BinaryExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}
};

struct DisjunctionExpression : public BinaryExpression,
                               public VisitableExpression<DisjunctionExpression> {
    DisjunctionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          BinaryExpression{std::move(lhs), std::move(rhs), position} {}
};

struct ConjunctionExpression : public BinaryExpression,
                               public VisitableExpression<ConjunctionExpression> {
    ConjunctionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          BinaryExpression{std::move(lhs), std::move(rhs), position} {}
};

struct ComparisonExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct EqualExpression : public ComparisonExpression,
                         public VisitableExpression<EqualExpression> {
    EqualExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          ComparisonExpression{std::move(lhs), std::move(rhs), position} {}
};

struct NotEqualExpression : public ComparisonExpression,
                            public VisitableExpression<NotEqualExpression> {
    NotEqualExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          ComparisonExpression{std::move(lhs), std::move(rhs), position} {}
};

struct RelationExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct LessThanExpression : public RelationExpression,
                            public VisitableExpression<LessThanExpression> {
    LessThanExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          RelationExpression{std::move(lhs), std::move(rhs), position} {}
};

struct LessThanOrEqualExpression : public RelationExpression,
                                   public VisitableExpression<LessThanOrEqualExpression> {
    LessThanOrEqualExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          RelationExpression{std::move(lhs), std::move(rhs), position} {}
};

struct GreaterThanExpression : public RelationExpression,
                               public VisitableExpression<GreaterThanExpression> {
    GreaterThanExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          RelationExpression{std::move(lhs), std::move(rhs), position} {}
};

struct GreaterThanOrEqualExpression
    : public RelationExpression,
      public VisitableExpression<GreaterThanOrEqualExpression> {
    GreaterThanOrEqualExpression(PExpression lhs, PExpression rhs,
                                 const Position& position)
        : SyntaxNode{position},
          RelationExpression{std::move(lhs), std::move(rhs), position} {}
};

struct AdditiveExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct AdditionExpression : public AdditiveExpression,
                            public VisitableExpression<AdditionExpression> {
    AdditionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          AdditiveExpression{std::move(lhs), std::move(rhs), position} {}
};

struct SubtractionExpression : public AdditiveExpression,
                               public VisitableExpression<SubtractionExpression> {
    SubtractionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          AdditiveExpression{std::move(lhs), std::move(rhs), position} {}
};

struct MultiplicativeExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct MultiplicationExpression : public MultiplicativeExpression,
                                  public VisitableExpression<MultiplicationExpression> {
    MultiplicationExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          MultiplicativeExpression{std::move(lhs), std::move(rhs), position} {}
};

struct DivisionExpression : public MultiplicativeExpression,
                            public VisitableExpression<DivisionExpression> {
    DivisionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          MultiplicativeExpression{std::move(lhs), std::move(rhs), position} {}
};

struct NegationExpression : public Expression {
    PExpression expr;
    NegationExpression(PExpression expr, const Position& position)
        : SyntaxNode{position}, expr{std::move(expr)} {}

    using Ctor = std::function<PExpression(PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct SignChangeExpression : public NegationExpression,
                              public VisitableExpression<SignChangeExpression> {
    SignChangeExpression(PExpression expr, const Position& position)
        : SyntaxNode{position}, NegationExpression{std::move(expr), position} {}
};

struct LogicalNegationExpression : public NegationExpression,
                                   public VisitableExpression<LogicalNegationExpression> {
    LogicalNegationExpression(PExpression expr, const Position& position)
        : SyntaxNode{position}, NegationExpression{std::move(expr), position} {}
};

struct TypeExpression : public Expression {
    PExpression expr;
    Type type;

    TypeExpression(PExpression expr, Type type, const Position& position)
        : SyntaxNode{position}, expr{std::move(expr)}, type{std::move(type)} {}

    using Ctor = std::function<PExpression(PExpression, Type, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct ConversionExpression : public TypeExpression,
                              public VisitableExpression<ConversionExpression> {
    ConversionExpression(PExpression expr, Type type, const Position& position)
        : SyntaxNode{position},
          TypeExpression{std::move(expr), std::move(type), position} {}
};

struct TypeCheckExpression : public TypeExpression,
                             public VisitableExpression<TypeCheckExpression> {
    TypeCheckExpression(PExpression expr, Type type, const Position& position)
        : SyntaxNode{position},
          TypeExpression{std::move(expr), std::move(type), position} {}
};

struct FieldAccessExpression : public Expression,
                               public VisitableExpression<FieldAccessExpression> {
    PExpression expr;
    std::string field;

    FieldAccessExpression(PExpression expr, std::string field, const Position& position)
        : SyntaxNode{position}, expr{std::move(expr)}, field{std::move(field)} {}
};

struct Constant : public Expression, public VisitableExpression<Constant> {
    using Value = std::variant<int, float, bool, std::string>;

    Value value;

    Constant(Value value, const Position& position)
        : SyntaxNode{position}, value{std::move(value)} {}
};

struct VariableAccess : public Expression, public VisitableExpression<VariableAccess> {
    std::string name;

    VariableAccess(std::string name, const Position& position)
        : SyntaxNode{position}, name{std::move(name)} {}
};

#endif
