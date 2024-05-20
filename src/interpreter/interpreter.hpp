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

    std::optional<ValueRef> getVariable(std::string_view name) const;
    std::optional<CallContext::FuncWithCtx> getFunctionWithCtx(
        std::string_view name) const;
    const StructDef* getStructDef(std::string_view name) const;
    const VariantDef* getVariantDef(std::string_view name) const;

    void operator()(const PrintStatement& stmt) const;
    void operator()(const VarDef& stmt);
    void operator()(const Assignment& stmt) const;
    void operator()(const FuncDef& stmt);
    void operator()(const FuncCall& stmt);
    void operator()(const StructDef& stmt);
    void operator()(const VariantDef& stmt);
    void operator()(const auto&) const { throw std::runtime_error("unknown statement"); }

   private:
    void addVariable(const std::string& name, ValueRef valueRef);
    void addFunction(const FuncDef* func);
    void addStruct(const StructDef* structDef);
    void addVariant(const VariantDef* variantDef);

    ValueRef convertToNamedStruct(ValueRef valueRef, const StructDef* structDef) const;
    ValueRef convertToVariant(ValueRef valueRef, const VariantDef* variantDef) const;

    ValueRef convertAndCheckType(const Type& expected, ValueRef valueRef) const;

    void passArguments(const Arguments& args, const Parameters& params);
    void passArgument(const Argument& arg, const Parameter& param);

    ValueRef getValueFromExpr(const Expression& expr) const;

    std::stack<CallContext> callStack_;
    std::ostream& out_;
};

#endif
