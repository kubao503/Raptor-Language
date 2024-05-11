#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parse_tree.hpp"

struct ExpressionInterpreter {
    Value operator()(const Constant& expr) const;
    Value operator()(const auto&) const { return {}; }
};

class Interpreter {
   public:
    Interpreter(const Program& program)
        : program_{program} {
        interpret();
    }

    void operator()(const PrintStatement& stmt) const;
    void operator()(const FuncDef& stmt);
    void operator()(const auto&) const { throw std::runtime_error("unknown statement"); }

   private:
    void interpret();

    using Pair = std::pair<std::string, const FuncDef*>;
    std::vector<Pair> functions_;
    const Program& program_;
};

struct ValuePrinter {
    void operator()(const std::monostate&) const {}
    void operator()(const auto& type) const;
};

#endif
