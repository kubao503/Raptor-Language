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

    void passArgumentsToCtx(CallContext& ctx, const Arguments& args,
                            const Parameters& params);
    void passArgumentToCtx(CallContext& ctx, const Argument& arg, const Parameter& param);

    ValueRef getValueFromExpr(const Expression& expr) const;

    std::stack<CallContext> callStack_;
    std::ostream& out_;
};

struct TypeComparer {
    bool operator()(BuiltInType variableType, Integral) {
        return variableType == BuiltInType::INT;
    }
    bool operator()(BuiltInType variableType, Floating) {
        return variableType == BuiltInType::FLOAT;
    }
    bool operator()(BuiltInType variableType, bool) {
        return variableType == BuiltInType::BOOL;
    }
    bool operator()(BuiltInType variableType, const std::string&) {
        return variableType == BuiltInType::STR;
    }
    bool operator()(const std::string& variableType, const NamedStructObj& structObj) {
        return structObj.structDef->name == variableType;
    }
    bool operator()(const std::string& variableType, const VariantObj& variantObj) {
        return variantObj.variantDef->name == variableType;
    }
    bool operator()(auto, auto) { return false; }
};

#endif
