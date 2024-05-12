#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stack>

#include "call_context.hpp"
#include "parse_tree.hpp"

class Interpreter;

class ExpressionInterpreter {
   public:
    ExpressionInterpreter(const Interpreter& interpreter);

    Value operator()(const Constant& expr) const;
    Value operator()(const VariableAccess& expr) const;
    Value operator()(const auto&) const {
        throw std::runtime_error("unknown expression");
    }

   private:
    const Interpreter& interpreter_;
};

class Interpreter {
   public:
    Interpreter(const Program& program)
        : program_{program} {
        callStack_.emplace(nullptr);
        interpret();
    }

    Value readVariable(std::string_view name) const;

    CallContext::FuncWithCtx getFunctionWithCtx(std::string_view name) const;

    void operator()(const PrintStatement& stmt) const;
    void operator()(const VarDef& stmt);
    void operator()(const FuncDef& stmt);
    void operator()(const FuncCall& stmt);
    void operator()(const auto&) const { throw std::runtime_error("unknown statement"); }

   private:
    void interpret();
    void addVariable(const std::string& name, ValueRef ref);
    void addFunction(const std::string& name, const FuncDef* func);

    const Program& program_;
    std::stack<CallContext> callStack_;
    std::vector<std::shared_ptr<ValueObj>> values_;
};

struct ValuePrinter {
    void operator()(const std::monostate&) const {}
    void operator()(const auto& type) const;
};

#endif
