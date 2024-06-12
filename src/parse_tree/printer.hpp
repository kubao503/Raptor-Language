#ifndef PRINTER_H
#define PRINTER_H

#include <ostream>

#include "parse_tree.hpp"

using namespace std::string_literals;

class ExpressionPrinter;
class StatementPrinter;

class BasePrinter {
   public:
    BasePrinter(unsigned indent = 0)
        : indent_{indent} {}

   protected:
    std::string getPrefix() const { return std::string(indent_, ' '); }

    ExpressionPrinter getSubExprPrinter() const;
    StatementPrinter getSubStmtPrinter() const;

    unsigned indent_{0};
    static constexpr unsigned indentWidth_{4};
};

class ExpressionPrinter : public BasePrinter, public ExpressionVisitor {
   public:
    using BasePrinter::BasePrinter;

    void operator()(const StructInitExpression& expr) const override;
    void operator()(const DisjunctionExpression& expr) const override;
    void operator()(const ConjunctionExpression& expr) const override;
    void operator()(const EqualExpression& expr) const override;
    void operator()(const NotEqualExpression& expr) const override;
    void operator()(const LessThanExpression& expr) const override;
    void operator()(const LessThanOrEqualExpression& expr) const override;
    void operator()(const GreaterThanExpression& expr) const override;
    void operator()(const GreaterThanOrEqualExpression& expr) const override;
    void operator()(const AdditionExpression& expr) const override;
    void operator()(const SubtractionExpression& expr) const override;
    void operator()(const MultiplicationExpression& expr) const override;
    void operator()(const DivisionExpression& expr) const override;
    void operator()(const SignChangeExpression&) const override;
    void operator()(const LogicalNegationExpression&) const override;
    void operator()(const ConversionExpression&) const override;
    void operator()(const TypeCheckExpression&) const override;
    void operator()(const FieldAccessExpression& expr) const override;
    void operator()(const Constant& expr) const override;
    void operator()(const FuncCall&) const override;
    void operator()(const VariableAccess& expr) const override;

   private:
    void printBinaryExpression(const auto& expression) const;
};

class StatementPrinter : public BasePrinter, public StatementVisitor {
   public:
    using BasePrinter::BasePrinter;

    void operator()(const IfStatement& ifStatement) override;
    void operator()(const WhileStatement& whileStatement) override;
    void operator()(const ReturnStatement& stmt) override;
    void operator()(const PrintStatement& stmt) override;
    void operator()(const FuncDef& funcDef) override;
    void operator()(const Assignment& stmt) override;
    void operator()(const VarDef& stmt) override;
    void operator()(const FuncCall& stmt) override;
    void operator()(const StructDef& stmt) override;
    void operator()(const VariantDef& stmt) override;
};

class LValuePrinter : public BasePrinter {
   public:
    using BasePrinter::BasePrinter;

    std::string operator()(const std::unique_ptr<FieldAccess>& lvalue) const;
    std::string operator()(const std::string& lvalue) const;
};

class TypePrinter : public BasePrinter {
   public:
    using BasePrinter::BasePrinter;

    std::string operator()(const std::string& type) const;
    std::string operator()(BuiltInType type) const;
};

struct ValuePrinter {
    std::string operator()(const std::monostate&) const;
    std::string operator()(const std::string& type) const;
    std::string operator()(const auto& type) const;
};

std::ostream& operator<<(std::ostream& stream, const Program& program);

#endif
