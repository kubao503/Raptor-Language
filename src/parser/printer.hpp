#ifndef PRINTER_H
#define PRINTER_H

#include <ostream>

#include "parse_tree.hpp"

using namespace std::string_literals;

struct ExpressionPrinter {};

class StatementPrinter {
   public:
    StatementPrinter(unsigned indent = 0)
        : indent_{indent} {}

    std::string operator()(const IfStatement& ifStatement) const;
    std::string operator()(const WhileStatement& whileStatement) const;
    std::string operator()(const FuncDef& funcDef) const;
    std::string operator()(const Assignment&) const;
    std::string operator()(const auto& stmt) const;

   private:
    std::string getPrefix() const { return std::string(indent_, ' '); }
    StatementPrinter createSubPrinter() const { return StatementPrinter(indent_ + 2); }

    unsigned indent_{0};
};

std::ostream& operator<<(std::ostream& stream, const Program& program);

#endif
