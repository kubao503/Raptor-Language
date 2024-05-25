#ifndef EXPR_INTERPRETER
#define EXPR_INTERPRETER

#include "parse_tree.hpp"
#include "value_obj.hpp"

class Interpreter;

class ExpressionInterpreter : public ExpressionVisitor {
   public:
    ExpressionInterpreter(Interpreter& interpreter);

    ValueRef getValue() const { return lastResult_; }

    void operator()(const StructInitExpression& expr) const override;
    void operator()(const DisjunctionExpression& expr) const override;
    void operator()(const ConjunctionExpression& expr) const override;
    void operator()(const EqualExpression& expr) const override;
    void operator()(const NotEqualExpression& expr) const override;
    void operator()(const LessThanExpression&) const override;
    void operator()(const LessThanOrEqualExpression&) const override;
    void operator()(const GreaterThanExpression&) const override;
    void operator()(const GreaterThanOrEqualExpression&) const override;
    void operator()(const AdditionExpression& expr) const override;
    void operator()(const SubtractionExpression& expr) const override;
    void operator()(const MultiplicationExpression& expr) const override;
    void operator()(const DivisionExpression& expr) const override;
    void operator()(const SignChangeExpression& expr) const override;
    void operator()(const LogicalNegationExpression& expr) const override;
    void operator()(const ConversionExpression& conversionExpr) const override;
    void operator()(const TypeCheckExpression&) const override;
    void operator()(const FieldAccessExpression& expr) const override;
    void operator()(const Constant& expr) const override;
    void operator()(const FuncCall& funcCall) const override;
    void operator()(const VariableAccess& expr) const override;

   private:
    std::pair<bool, bool> getBoolExpression(Expression* lhs, Expression* rhs) const;
    void comparison(const BinaryExpression& expr) const;

    template <typename Functor>
    void evalNumericExpr(const BinaryExpression& expr, Functor func) const;

    Interpreter& interpreter_;
    mutable ValueRef lastResult_;
};

#endif
