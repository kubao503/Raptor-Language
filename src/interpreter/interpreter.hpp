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
    const StructDef* getStructDef(std::string_view name) const;

    void operator()(const PrintStatement& stmt) const;
    void operator()(const VarDef& stmt);
    void operator()(const Assignment& stmt) const;
    void operator()(const FuncDef& stmt);
    void operator()(const FuncCall& stmt);
    void operator()(const StructDef& stmt);
    void operator()(const auto&) const { throw std::runtime_error("unknown statement"); }

   private:
    void addVariable(const std::string& name, ValueRef valueRef);
    void addFunction(const FuncDef* func);
    void addStruct(const StructDef* structDef);

    NamedStructObj nameStructObj(const StructObj& structObj, std::string_view name) const;
    ValueRef convertToNamed(const Type& varDefType, ValueRef valueRef) const;
    ValueRef convertToNamed(ValueRef oldValueRef, ValueRef newValueRef) const;

    void passArguments(const Arguments& args, const Parameters& params);
    void passArgument(const Argument& arg, const Parameter& param);

    std::optional<ValueRef> getValueFromExpr(const Expression* expr) const;

    std::stack<CallContext> callStack_;
    std::ostream& out_;
};

#endif
