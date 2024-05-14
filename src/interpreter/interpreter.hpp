#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <ostream>
#include <stack>

#include "call_context.hpp"
#include "parse_tree.hpp"

class Interpreter {
   public:
    Interpreter(const Program& program, std::ostream& out);

    ValueRef readVariable(std::string_view name) const;

    CallContext::FuncWithCtx getFunctionWithCtx(std::string_view name) const;

    void operator()(const PrintStatement& stmt) const;
    void operator()(const VarDef& stmt);
    void operator()(const Assignment& stmt) const;
    void operator()(const FuncDef& stmt);
    void operator()(const FuncCall& stmt);
    void operator()(const auto&) const { throw std::runtime_error("unknown statement"); }

   private:
    void interpret();
    void addVariable(const std::string& name, ValueRef ref);
    void addFunction(const std::string& name, const FuncDef* func);
    void passArguments(const Arguments& args, const Parameters& params);
    void passArgument(const Argument& arg, const Parameter& param);

    Value getValueFromExpr(const Expression* expr) const;

    const Program& program_;
    std::stack<CallContext> callStack_;
    std::ostream& out_;
};

struct ValuePrinter {
    ValuePrinter(std::ostream& out)
        : out_{out} {}

    void operator()(const std::monostate&) const {}
    void operator()(const auto& type) const;

   private:
    std::ostream& out_;
};

#endif
