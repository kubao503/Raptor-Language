#ifndef EXPR_INTERPRETER
#define EXPR_INTERPRETER

#include "parse_tree.hpp"
#include "value_obj.hpp"

class Interpreter;

class ExpressionInterpreter : public ExpressionVisitor {
   public:
    ExpressionInterpreter(const Interpreter& interpreter);

    ValueRef getValue() const { return lastResult_; }

    void operator()(const StructInitExpression& expr) const override;
    void operator()(const DisjunctionExpression&) const override;
    void operator()(const ConjunctionExpression&) const override;
    void operator()(const EqualExpression&) const override;
    void operator()(const NotEqualExpression&) const override;
    void operator()(const LessThanExpression&) const override;
    void operator()(const LessThanOrEqualExpression&) const override;
    void operator()(const GreaterThanExpression&) const override;
    void operator()(const GreaterThanOrEqualExpression&) const override;
    void operator()(const AdditionExpression&) const override;
    void operator()(const SubtractionExpression&) const override;
    void operator()(const MultiplicationExpression&) const override;
    void operator()(const DivisionExpression&) const override;
    void operator()(const SignChangeExpression&) const override;
    void operator()(const LogicalNegationExpression&) const override;
    void operator()(const ConversionExpression&) const override;
    void operator()(const TypeCheckExpression&) const override;
    void operator()(const FieldAccessExpression& expr) const override;
    void operator()(const Constant& expr) const override;
    void operator()(const FuncCall&) const override;
    void operator()(const VariableAccess& expr) const override;

   private:
    const Interpreter& interpreter_;
    mutable ValueRef lastResult_;
};

#endif
