#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <ostream>
#include <stack>

#include "call_context.hpp"
#include "expr_interpreter.hpp"
#include "parse_tree.hpp"

using ReturnValue = std::optional<ValueObj>;

/// @brief Statement interpreter
class Interpreter : public StatementVisitor {
   public:
    /// @brief
    /// @param out the stream to which the output will be written
    explicit Interpreter(std::ostream& out);

    /// @brief Interprets the given program
    /// @param program
    void interpret(const Program& program);

    /// @brief Returns a reference to a variable with the given name or std::nullopt if
    /// not found
    /// @param name
    std::optional<RefObj> getVariable(std::string_view name) const;

    /// @brief Returns a function definition with the given name along with the scope in
    /// which the function is defined. If not found the std::nullopt is returned
    /// @param name
    std::optional<CallContext::FuncWithCtx> getFunctionWithCtx(
        std::string_view name) const;

    /// @brief Returns a function definition with the given name or a nullptr if nout
    /// found
    /// @param name
    const StructDef* getStructDef(std::string_view name) const;

    /// @brief Returns a variant definition with the given name or a nullptr if nout found
    /// @param name
    const VariantDef* getVariantDef(std::string_view name) const;

    void operator()(const IfStatement& ifStmt) override;
    void operator()(const WhileStatement& whileStmt) override;
    void operator()(const ReturnStatement& stmt) override;
    void operator()(const PrintStatement& stmt) override;
    void operator()(const VarDef& stmt) override;
    void operator()(const Assignment& stmt) override;
    void operator()(const FuncDef& stmt) override;
    void operator()(const FuncCall& funcCall) override;
    void operator()(const StructDef& stmt) override;
    void operator()(const VariantDef& stmt) override;

    /// @brief Executes the given function call and returns the returned value
    /// @param funcCall
    /// @return Value returned from the function call
    ReturnValue handleFunctionCall(const FuncCall& funcCall);

   private:
    void addVariable(VarEntry entry);
    void addFunction(const FuncDef* func);
    void addStruct(const StructDef* structDef);
    void addVariant(const VariantDef* variantDef);

    ValueHolder convertAndCheckType(const Type& expected, ValueHolder holder) const;
    void convertToUserDefinedType(ValueObj& valueObj, std::string_view typeName) const;

    /// @brief Converts anonymous struct (StructObj) to one with field names (NamedStruct)
    /// @param valueObj
    /// @param structDef
    void convertToNamedStruct(ValueObj& valueObj, const StructDef* structDef) const;

    /// @brief Converts value to variant if the value type matches one of the variant's
    /// types
    /// @param valueObj
    /// @param variantDef
    void convertToVariant(ValueObj& valueObj, const VariantDef* variantDef) const;

    RefObj tryAccessLValue(const Assignment& stmt) const;

    bool evaluateCondition(const ConditionalStatement& stmt);
    void interpretStatementsInNewContext(const Statements& statements);

    void passArgumentsToCtx(CallContext& ctx, const Arguments& args,
                            const Parameters& params);
    void passArgumentToCtx(CallContext& ctx, const Argument& arg, const Parameter& param);

    ValueHolder getValueFromExpr(const Expression& expr);

    std::stack<CallContext> callStack_;
    std::ostream& out_;

    ExpressionInterpreter exprInterpreter_{this};
    ReturnValue returnValue_{std::nullopt};
    bool returning_{false};
};

/// @brief Checks if the given value is of the given type
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

/// @brief Returns the type of the given value
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
