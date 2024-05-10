#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "expressions.hpp"

struct IfStatement;
struct WhileStatement;

struct ReturnStatement;
struct PrintStatement;

struct FieldAccess;

using LValue = std::variant<std::string, std::unique_ptr<FieldAccess>>;

struct FieldAccess {
    LValue container;
    std::string field;
};

struct Assignment {
    LValue lhs;
    Expression rhs;
};

struct VarDef {
    bool isConst{false};
    Type type{""};
    std::string name;
    Expression expression;
};

struct Parameter {
    Type type{""};
    std::string name;
    bool ref{false};
};

using Parameters = std::vector<Parameter>;

struct FuncDef;

struct Field {
    Type type{""};
    std::string name;
};

struct StructDef {
    std::string name;
    std::vector<Field> fields;
};

struct VariantDef {
    class NoTypesException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    std::string name;
    std::vector<Type> types;

    VariantDef(std::string name, std::vector<Type> types)
        : name{std::move(name)}, types{std::move(types)} {
        if (this->types.empty())
            throw NoTypesException("Expected at least one type in variant definition");
    }
};

using Statement =
    std::variant<IfStatement, WhileStatement, ReturnStatement, PrintStatement, FuncDef,
                 Assignment, VarDef, FuncCall, StructDef, VariantDef>;
using Statements = std::vector<Statement>;

struct IfStatement {
    Expression condition;
    Statements statements;
};

struct WhileStatement {
    Expression condition;
    Statements statements;
};

struct ReturnStatement {
    std::optional<Expression> expression;
};

struct PrintStatement {
    std::optional<Expression> expression;
};

class FuncDef {
   public:
    FuncDef(const ReturnType& returnType, const std::string& name,
            const Parameters& parameters, Statements statements, const Position& position)
        : returnType_{returnType},
          name_{name},
          parameters_{parameters},
          statements_{std::move(statements)},
          position_{position} {}

    const ReturnType& getReturnType() const { return returnType_; }
    const std::string& getName() const { return name_; }
    const Parameters& getParameters() const { return parameters_; }
    const Statements& getStatements() const { return statements_; }

   private:
    ReturnType returnType_{""};
    std::string name_;
    Parameters parameters_;
    Statements statements_;
    Position position_;
};

#endif
