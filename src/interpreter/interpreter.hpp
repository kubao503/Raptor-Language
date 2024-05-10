#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parse_tree.hpp"

void interpret(const Program& program);

struct ExpressionInterpreter {
    Value operator()(const Constant& expr) const;
    Value operator()(const auto&) const { return {}; }
};

struct Interpreter {
    void operator()(const PrintStatement& stmt) const;
    void operator()(const auto&) const { throw std::runtime_error("unknown statement"); }
};

struct ValuePrinter {
    void operator()(const std::monostate&) const {}
    void operator()(const std::string& type) const;
    void operator()(const auto& type) const;
};

#endif
