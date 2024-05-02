#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "position.hpp"
#include "types.hpp"

enum class BuiltInType {
    INT,
    FLOAT,
    BOOL,
    STR,
};

struct VoidType {};

using Type = std::variant<std::string, BuiltInType>;
using ReturnType = std::variant<std::string, BuiltInType, VoidType>;

struct Constant {
    Value value;
};

struct DisjunctionExpression;
struct ConjunctionExpression;
struct EqualExpression;
struct NotEqualExpression;

using Expression =
    std::variant<Constant, std::unique_ptr<DisjunctionExpression>,
                 std::unique_ptr<ConjunctionExpression>, std::unique_ptr<EqualExpression>,
                 std::unique_ptr<NotEqualExpression>>;

struct DisjunctionExpression {
    Expression lhs;
    Expression rhs;
};

struct ConjunctionExpression {
    Expression lhs;
    Expression rhs;
};

struct EqualExpression {
    Expression lhs;
    Expression rhs;
};

struct NotEqualExpression {
    Expression lhs;
    Expression rhs;
};

struct IfStatement {
    Expression condition;
};

struct Assignment {
    std::string lhs;
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

using Statement = std::variant<IfStatement, FuncDef, Assignment, VarDef>;
using Statements = std::vector<Statement>;

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
    std::string_view getName() const { return name_; }
    const Parameters& getParameters() const { return parameters_; }
    const Statements& getStatements() const { return statements_; }

   private:
    ReturnType returnType_{""};
    std::string name_;
    Parameters parameters_;
    Statements statements_;
    Position position_;
};

struct Program {
    Statements statements;
};

#endif
