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

struct SyntaxNode {
    virtual ~SyntaxNode() = default;

    SyntaxNode(const Position& position)
        : position{position} {}

    Position position;
};

struct Expression : public virtual SyntaxNode {
    virtual void accept(const ExpressionVisitor& vis) const = 0;
};

using PExpression = std::unique_ptr<Expression>;

struct StructInitExpression : public Expression {
    std::vector<PExpression> exprs;

    StructInitExpression(std::vector<PExpression> exprs, const Position& position)
        : SyntaxNode{position}, exprs{std::move(exprs)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct BinaryExpression : public Expression {
    PExpression lhs;
    PExpression rhs;

    BinaryExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}
};

struct DisjunctionExpression : public BinaryExpression {
    DisjunctionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          BinaryExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct ConjunctionExpression : public BinaryExpression {
    ConjunctionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          BinaryExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct ComparisonExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct EqualExpression : public ComparisonExpression {
    EqualExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          ComparisonExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct NotEqualExpression : public ComparisonExpression {
    NotEqualExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          ComparisonExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct RelationExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct LessThanExpression : public RelationExpression {
    LessThanExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          RelationExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct LessThanOrEqualExpression : public RelationExpression {
    LessThanOrEqualExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          RelationExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct GreaterThanExpression : public RelationExpression {
    GreaterThanExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          RelationExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct GreaterThanOrEqualExpression : public RelationExpression {
    GreaterThanOrEqualExpression(PExpression lhs, PExpression rhs,
                                 const Position& position)
        : SyntaxNode{position},
          RelationExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct AdditiveExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct AdditionExpression : public AdditiveExpression {
    AdditionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          AdditiveExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct SubtractionExpression : public AdditiveExpression {
    SubtractionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          AdditiveExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct MultiplicativeExpression : public BinaryExpression {
    using BinaryExpression::BinaryExpression;
    using Ctor = std::function<PExpression(PExpression, PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct MultiplicationExpression : public MultiplicativeExpression {
    MultiplicationExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          MultiplicativeExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct DivisionExpression : public MultiplicativeExpression {
    DivisionExpression(PExpression lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position},
          MultiplicativeExpression{std::move(lhs), std::move(rhs), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct NegationExpression : public Expression {
    PExpression expr;
    NegationExpression(PExpression expr, const Position& position)
        : SyntaxNode{position}, expr{std::move(expr)} {}

    using Ctor = std::function<PExpression(PExpression, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct SignChangeExpression : public NegationExpression {
    SignChangeExpression(PExpression expr, const Position& position)
        : SyntaxNode{position}, NegationExpression{std::move(expr), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct LogicalNegationExpression : public NegationExpression {
    LogicalNegationExpression(PExpression expr, const Position& position)
        : SyntaxNode{position}, NegationExpression{std::move(expr), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct TypeExpression : public Expression {
    PExpression expr;
    Type type;

    TypeExpression(PExpression expr, Type type, const Position& position)
        : SyntaxNode{position}, expr{std::move(expr)}, type{std::move(type)} {}

    using Ctor = std::function<PExpression(PExpression, Type, const Position&)>;

    static std::optional<Ctor> getCtor(Token::Type type);
};

struct ConversionExpression : public TypeExpression {
    ConversionExpression(PExpression expr, Type type, const Position& position)
        : SyntaxNode{position},
          TypeExpression{std::move(expr), std::move(type), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct TypeCheckExpression : public TypeExpression {
    TypeCheckExpression(PExpression expr, Type type, const Position& position)
        : SyntaxNode{position},
          TypeExpression{std::move(expr), std::move(type), position} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct FieldAccessExpression : public Expression {
    PExpression expr;
    std::string field;

    FieldAccessExpression(PExpression expr, std::string field, const Position& position)
        : SyntaxNode{position}, expr{std::move(expr)}, field{std::move(field)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct Constant : public Expression {
    using Value = std::variant<int, float, bool, std::string>;

    Value value;

    Constant(Value value, const Position& position)
        : SyntaxNode{position}, value{std::move(value)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

struct VariableAccess : public Expression {
    std::string name;

    VariableAccess(std::string name, const Position& position)
        : SyntaxNode{position}, name{std::move(name)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
};

#endif
