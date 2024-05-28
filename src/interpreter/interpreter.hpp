#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <ostream>
#include <stack>

#include "call_context.hpp"
#include "parse_tree.hpp"

using ReturnValue = std::optional<ValueObj>;

class Interpreter {
   public:
    explicit Interpreter(std::ostream& out);
    void interpret(const Program& program);

    std::optional<RefObj> getVariable(std::string_view name) const;
    std::optional<CallContext::FuncWithCtx> getFunctionWithCtx(
        std::string_view name) const;
    const StructDef* getStructDef(std::string_view name) const;
    const VariantDef* getVariantDef(std::string_view name) const;

    void operator()(const IfStatement& ifStmt);
    void operator()(const WhileStatement& whileStmt);
    void operator()(const ReturnStatement& stmt);
    void operator()(const PrintStatement& stmt);
    void operator()(const VarDef& stmt);
    void operator()(const Assignment& stmt);
    void operator()(const FuncDef& stmt);
    void operator()(const FuncCall& funcCall);
    void operator()(const StructDef& stmt);
    void operator()(const VariantDef& stmt);

    ReturnValue handleFunctionCall(const FuncCall& funcCall);

   private:
    void addVariable(VarEntry entry);
    void addFunction(const FuncDef* func);
    void addStruct(const StructDef* structDef);
    void addVariant(const VariantDef* variantDef);

    ValueHolder convertAndCheckType(const Type& expected, ValueHolder valueRef) const;
    void convertToUserDefinedType(ValueObj& valueObj, std::string_view typeName) const;
    void convertToNamedStruct(ValueObj& valueObj, const StructDef* structDef) const;
    void convertToVariant(ValueObj& valueObj, const VariantDef* variantDef) const;

    RefObj tryAccessField(const Assignment& stmt) const;

    template <typename ConditionalStatement>
    bool evaluateCondition(const ConditionalStatement& stmt);
    void interpretStatements(const Statements& statements);

    void passArgumentsToCtx(CallContext& ctx, const Arguments& args,
                            const Parameters& params);
    void passArgumentToCtx(CallContext& ctx, const Argument& arg, const Parameter& param);

    ValueHolder getValueFromExpr(const Expression& expr);

    std::stack<CallContext> callStack_;
    std::ostream& out_;

    ReturnValue returnValue_;
    bool returning_{false};
};

struct TypeComparer {
    bool operator()(BuiltInType variableType, Integral) const {
        return variableType == BuiltInType::INT;
    }
    bool operator()(BuiltInType variableType, Floating) const {
        return variableType == BuiltInType::FLOAT;
    }
    bool operator()(BuiltInType variableType, bool) const {
        return variableType == BuiltInType::BOOL;
    }
    bool operator()(BuiltInType variableType, const std::string&) const {
        return variableType == BuiltInType::STR;
    }
    bool operator()(const std::string& variableType,
                    const NamedStructObj& structObj) const {
        return structObj.structDef->name == variableType;
    }
    bool operator()(const std::string& variableType, const VariantObj& variantObj) const {
        return variantObj.variantDef->name == variableType;
    }
    bool operator()(const auto&, const auto&) const { return false; }
};

struct ValueToType {
    Type operator()(Integral) const { return BuiltInType::INT; }
    Type operator()(Floating) const { return BuiltInType::FLOAT; }
    Type operator()(bool) const { return BuiltInType::BOOL; }
    Type operator()(const std::string&) const { return BuiltInType::STR; }
    Type operator()(const NamedStructObj& structObj) const {
        return structObj.structDef->name;
    }
    Type operator()(const VariantObj& variantObj) const {
        return variantObj.variantDef->name;
    }
    Type operator()(const StructObj&) const { return "Anonymous struct"; }
};

#endif
