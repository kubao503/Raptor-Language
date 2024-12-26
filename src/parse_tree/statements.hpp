#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "expressions.hpp"

struct IfStatement;
struct WhileStatement;

struct ReturnStatement;
struct PrintStatement;

class FuncDef;
struct Assignment;
struct VarDef;
struct FuncCall;

struct StructDef;
struct VariantDef;

using StatementVisitor =
    Visitor<IfStatement, WhileStatement, ReturnStatement, PrintStatement, FuncDef,
            Assignment, VarDef, FuncCall, StructDef, VariantDef>;

struct Statement : public virtual SyntaxNode,
                   public virtual Visitable<StatementVisitor> {};

using PStatement = std::unique_ptr<Statement>;
using Statements = std::vector<PStatement>;

template <typename T>
using VisitableStatement = VisitableImpl<T, StatementVisitor>;

struct ConditionalStatement : public Statement {
    ConditionalStatement(PExpression condition, Statements statements,
                         const Position& position)
        : SyntaxNode{position},
          condition{std::move(condition)},
          statements{std::move(statements)} {}

    PExpression condition;
    Statements statements;
};

struct IfStatement : public ConditionalStatement, public VisitableStatement<IfStatement> {
    IfStatement(PExpression condition, Statements statements, const Position& position)
        : SyntaxNode{position},
          ConditionalStatement{std::move(condition), std::move(statements), position} {}
};

struct WhileStatement : public ConditionalStatement,
                        public VisitableStatement<WhileStatement> {
    WhileStatement(PExpression condition, Statements statements, const Position& position)
        : SyntaxNode{position},
          ConditionalStatement{std::move(condition), std::move(statements), position} {}
};

struct ReturnStatement : public Statement, public VisitableStatement<ReturnStatement> {
    ReturnStatement(PExpression expression, const Position& position)
        : SyntaxNode{position}, expression{std::move(expression)} {}

    PExpression expression;
};

struct PrintStatement : public Statement, public VisitableStatement<PrintStatement> {
    PrintStatement(PExpression expression, const Position& position)
        : SyntaxNode{position}, expression{std::move(expression)} {}

    PExpression expression;
};

struct Parameter {
    Type type{""};
    std::string name;
    bool ref{false};
    Position position;
};

using Parameters = std::vector<Parameter>;

class FuncDef : public Statement, public VisitableStatement<FuncDef> {
   public:
    FuncDef(const ReturnType& returnType, const std::string& name,
            const Parameters& parameters, Statements statements, const Position& position)
        : SyntaxNode{position},
          returnType_{returnType},
          name_{name},
          parameters_{parameters},
          statements_{std::move(statements)} {}

    const ReturnType& getReturnType() const { return returnType_; }
    const std::string& getName() const { return name_; }
    const Parameters& getParameters() const { return parameters_; }
    const Statements& getStatements() const { return statements_; }

   private:
    ReturnType returnType_{""};
    std::string name_;
    Parameters parameters_;
    Statements statements_;
};

struct FieldAccess;

/// @brief Left hand side of the assignment statement
using LValue = std::variant<std::string, std::unique_ptr<FieldAccess>>;

struct FieldAccess {
    LValue container;
    std::string field;
};

struct Assignment : public Statement, public VisitableStatement<Assignment> {
    Assignment(LValue lhs, PExpression rhs, const Position& position)
        : SyntaxNode{position}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}

    LValue lhs;
    PExpression rhs;
};

struct VarDef : public Statement, public VisitableStatement<VarDef> {
    VarDef(bool isConst, Type type, std::string name, PExpression expression,
           const Position& position)
        : SyntaxNode{position},
          isConst{isConst},
          type{std::move(type)},
          name{std::move(name)},
          expression{std::move(expression)} {}

    bool isConst;
    Type type;
    std::string name;
    PExpression expression;
};

struct Argument {
    PExpression value;
    bool ref{false};
    Position position;
};

using Arguments = std::vector<Argument>;

struct FuncCall : public Expression,
                  public Statement,
                  public VisitableExpression<FuncCall>,
                  public VisitableStatement<FuncCall> {
    std::string name;
    Arguments arguments;

    FuncCall(std::string name, Arguments arguments, const Position& position)
        : SyntaxNode{position}, name{std::move(name)}, arguments{std::move(arguments)} {}
};

struct Field {
    Type type{""};
    std::string name;
};

struct StructDef : public Statement, public VisitableStatement<StructDef> {
    StructDef(std::string name, std::vector<Field> fields, const Position& position)
        : SyntaxNode{position}, name{std::move(name)}, fields{std::move(fields)} {}

    std::string name;
    std::vector<Field> fields;
};

struct VariantDef : public Statement, public VisitableStatement<VariantDef> {
    VariantDef(std::string name, std::vector<Type> types, const Position& position)
        : SyntaxNode{position}, name{std::move(name)}, types{std::move(types)} {}

    std::string name;
    std::vector<Type> types;
};

#endif
