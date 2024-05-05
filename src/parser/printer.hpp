#ifndef PRINTER_H
#define PRINTER_H

#include <ostream>

#include "parse_tree.hpp"

using namespace std::string_literals;

class BasePrinter {
   public:
    BasePrinter(unsigned indent = 0)
        : indent_{indent} {}

   protected:
    std::string getPrefix() const { return std::string(indent_, ' '); }

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
    std::string operator()(const auto& expr) const;
};

class StatementPrinter : public BasePrinter {
   public:
    using BasePrinter::BasePrinter;

    std::string operator()(const IfStatement& ifStatement) const;
    std::string operator()(const WhileStatement& whileStatement) const;
    std::string operator()(const FuncDef& funcDef) const;
    std::string operator()(const Assignment&) const;
    std::string operator()(const auto& stmt) const;
};

std::ostream& operator<<(std::ostream& stream, const Program& program);

#endif
