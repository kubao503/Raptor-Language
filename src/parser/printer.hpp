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

class ExpressionPrinter : public BasePrinter {
   public:
    using BasePrinter::BasePrinter;

    std::string operator()(
        const std::unique_ptr<DisjunctionExpression>& disjunction) const;
    std::string operator()(
        const std::unique_ptr<ConjunctionExpression>& conjunction) const;
    std::string operator()(const std::unique_ptr<EqualExpression>& expr) const;
    std::string operator()(const std::unique_ptr<NotEqualExpression>& expr) const;
    std::string operator()(const std::unique_ptr<LessThanExpression>& expr) const;
    std::string operator()(const std::unique_ptr<LessThanOrEqualExpression>& expr) const;
    std::string operator()(const std::unique_ptr<GreaterThanExpression>& expr) const;
    std::string operator()(
        const std::unique_ptr<GreaterThanOrEqualExpression>& expr) const;
    std::string operator()(const std::unique_ptr<AdditionExpression>& expr) const;
    std::string operator()(const std::unique_ptr<SubtractionExpression>& expr) const;
    std::string operator()(const std::unique_ptr<MultiplicationExpression>& expr) const;
    std::string operator()(const std::unique_ptr<DivisionExpression>& expr) const;
    std::string operator()(const auto& expr) const;

   private:
    std::string printBinaryExpression(const auto& expression) const;
};

class StatementPrinter : public BasePrinter {
   public:
    using BasePrinter::BasePrinter;

    std::string operator()(const IfStatement& ifStatement) const;
    std::string operator()(const WhileStatement& whileStatement) const;
    std::string operator()(const ReturnStatement& stmt) const;
    std::string operator()(const PrintStatement& stmt) const;
    std::string operator()(const FuncDef& funcDef) const;
    std::string operator()(const Assignment&) const;
    // std::string operator()(const VarDef& stmt) const;
    // std::string operator()(const FuncCall& stmt) const;
    // std::string operator()(const StructDef& stmt) const;
    // std::string operator()(const VariantDef& stmt) const;
    std::string operator()(const auto& stmt) const;

   private:
    std::string printIfOrWhileStatement(const IfOrWhileStatement& ifOrWhile) const;
    std::string printReturnOrPrintStatement(const ReturnOrPrintStatement& stmt) const;
};

std::ostream& operator<<(std::ostream& stream, const Program& program);

#endif
