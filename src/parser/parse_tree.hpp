#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <variant>
#include <vector>

#include "token.hpp"

struct IfStatement {};

struct Assignment {
    std::string lhs;
    Token::Value rhs;
};

using Type = std::variant<Token::Type, std::string>;

struct VarDef {
    bool isConst{false};
    Type type{Token::Type::UNKNOWN};
    std::string name;
    Token::Value value;
};

struct Parameter {
    Type type{Token::Type::UNKNOWN};
    std::string name;
    bool ref{false};
};

using Parameters = std::vector<Parameter>;

struct FuncDef;

using Statement = std::variant<IfStatement, FuncDef, Assignment, VarDef>;
using Statements = std::vector<Statement>;

class FuncDef {
   public:
    FuncDef(Type returnType, const std::string& name, const Parameters& parameters,
            Statements statements, Position position)
        : returnType_{returnType},
          name_{name},
          parameters_{parameters},
          statements_{statements},
          position_{position} {}

    Type getReturnType() const { return returnType_; }
    std::string_view getName() const { return name_; }
    const Parameters& getParameters() const { return parameters_; }
    const Statements& getStatements() const { return statements_; }

   private:
    Type returnType_{Token::Type::UNKNOWN};
    std::string name_;
    Parameters parameters_;
    Statements statements_;
    Position position_;
};

struct Program {
    Statements statements;
};

#endif
