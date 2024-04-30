#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <variant>
#include <vector>

struct IfStatement {};

struct Assignment {
    std::string lhs;
    Token::Value rhs;
};

struct Parameter {
    Token::Type type;
    std::string name;
    bool ref;
};

using Parameters = std::vector<Parameter>;

struct FuncDef;

using Statement = std::variant<IfStatement, FuncDef, Assignment>;
using Statements = std::vector<Statement>;

class FuncDef {
   public:
    FuncDef(Token::Type returnType, const std::string& name, const Parameters& parameters,
            Statements statements, Position position)
        : returnType_{returnType},
          name_{name},
          parameters_{parameters},
          statements_{statements},
          position_{position} {}

    Token::Type getReturnType() const { return returnType_; }
    std::string_view getName() const { return name_; }
    const Parameters& getParameters() const { return parameters_; }
    const Statements& getStatements() const { return statements_; }

   private:
    Token::Type returnType_{Token::Type::UNKNOWN};
    std::string name_;
    Parameters parameters_;
    Statements statements_;
    Position position_;
};

#endif
