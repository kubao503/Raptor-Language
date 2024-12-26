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

struct StatementVisitor {
    virtual void operator()(const IfStatement& stmt) = 0;
    virtual void operator()(const WhileStatement& stmt) = 0;
    virtual void operator()(const ReturnStatement& stmt) = 0;
    virtual void operator()(const PrintStatement& stmt) = 0;
    virtual void operator()(const FuncDef& stmt) = 0;
    virtual void operator()(const Assignment& stmt) = 0;
    virtual void operator()(const VarDef& stmt) = 0;
    virtual void operator()(const FuncCall& stmt) = 0;
    virtual void operator()(const StructDef& stmt) = 0;
    virtual void operator()(const VariantDef& stmt) = 0;
};

struct Statement {
    Position position;

    Statement(Position position)
        : position{position} {}

    virtual ~Statement() = default;
    virtual void accept(StatementVisitor& vis) const = 0;
};

using PStatement = std::unique_ptr<Statement>;
using Statements = std::vector<PStatement>;

struct ConditionalStatement : public Statement {
    ConditionalStatement(PExpression condition, Statements statements,
                         const Position& position)
        : Statement{position},
          condition{std::move(condition)},
          statements{std::move(statements)} {}

    PExpression condition;
    Statements statements;
};

struct IfStatement : public ConditionalStatement {
    using ConditionalStatement::ConditionalStatement;

    void accept(StatementVisitor& vis) const override { vis(*this); }
};

struct WhileStatement : public ConditionalStatement {
    using ConditionalStatement::ConditionalStatement;

    void accept(StatementVisitor& vis) const override { vis(*this); }
};

struct ReturnStatement : public Statement {
    PExpression expression;

    ReturnStatement(PExpression expression, const Position& position)
        : Statement{position}, expression{std::move(expression)} {}

    void accept(StatementVisitor& vis) const override { vis(*this); }
};

struct PrintStatement : public Statement {
    PExpression expression;

    PrintStatement(PExpression expression, const Position& position)
        : Statement{position}, expression{std::move(expression)} {}

    void accept(StatementVisitor& vis) const override { vis(*this); }
};

struct Parameter {
    Type type{""};
    std::string name;
    bool ref{false};
    Position position;
};

using Parameters = std::vector<Parameter>;

class FuncDef : public Statement {
   public:
    FuncDef(const ReturnType& returnType, const std::string& name,
            const Parameters& parameters, Statements statements, const Position& position)
        : Statement{position},
          returnType_{returnType},
          name_{name},
          parameters_{parameters},
          statements_{std::move(statements)} {}

    void accept(StatementVisitor& vis) const override { vis(*this); }

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

struct Assignment : public Statement {
    Assignment(LValue lhs, PExpression rhs, const Position& position)
        : Statement{position}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}

    void accept(StatementVisitor& vis) const override { vis(*this); }

    LValue lhs;
    PExpression rhs;
};

struct VarDef : public Statement {
    VarDef(bool isConst, Type type, std::string name, PExpression expression,
           const Position& position)
        : Statement{position},
          isConst{isConst},
          type{std::move(type)},
          name{std::move(name)},
          expression{std::move(expression)} {}

    void accept(StatementVisitor& vis) const override { vis(*this); }

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

struct FuncCall : public Expression, public Statement {
    std::string name;
    Arguments arguments;

    FuncCall(std::string name, Arguments arguments, const Position& position)
        : Expression{position},
          Statement{position},
          name{std::move(name)},
          arguments{std::move(arguments)} {}

    void accept(const ExpressionVisitor& vis) const override { vis(*this); }
    void accept(StatementVisitor& vis) const override { vis(*this); }
};

struct Field {
    Type type{""};
    std::string name;
};

struct StructDef : public Statement {
    StructDef(std::string name, std::vector<Field> fields, const Position& position)
        : Statement{position}, name{std::move(name)}, fields{std::move(fields)} {}

    void accept(StatementVisitor& vis) const override { vis(*this); }

    std::string name;
    std::vector<Field> fields;
};

struct VariantDef : public Statement {
    VariantDef(std::string name, std::vector<Type> types, const Position& position)
        : Statement{position}, name{std::move(name)}, types{std::move(types)} {}

    void accept(StatementVisitor& vis) const override { vis(*this); }

    std::string name;
    std::vector<Type> types;
};

#endif
