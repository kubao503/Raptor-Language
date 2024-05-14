#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <ostream>
#include <stack>

#include "call_context.hpp"
#include "parse_tree.hpp"

class Interpreter {
   public:
    Interpreter(std::ostream& out);
    void interpret(const Program& program);

    ValueRef readVariable(std::string_view name) const;

    CallContext::FuncWithCtx getFunctionWithCtx(std::string_view name) const;

    void operator()(const PrintStatement& stmt) const;
    void operator()(const VarDef& stmt);
    void operator()(const Assignment& stmt) const;
    void operator()(const FuncDef& stmt);
    void operator()(const FuncCall& stmt);
    void operator()(const auto&) const { throw std::runtime_error("unknown statement"); }

   private:
    void addVariable(const std::string& name, ValueRef ref);
    void addFunction(const std::string& name, const FuncDef* func);
    void passArguments(const Arguments& args, const Parameters& params);
    void passArgument(const Argument& arg, const Parameter& param);

    std::optional<Value> getValueFromExpr(const Expression* expr) const;

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

struct ValueToType {
    Type operator()(Integral) const { return BuiltInType::INT; }
    Type operator()(Floating) const { return BuiltInType::FLOAT; }
    Type operator()(bool) const { return BuiltInType::BOOL; }
    Type operator()(const std::string&) const { return BuiltInType::STR; }
    Type operator()(const std::monostate&) const { throw std::runtime_error("No value"); }
};

#endif
