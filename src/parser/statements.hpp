#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <variant>
#include <vector>

struct IfStatement {};

struct Param {};

struct FuncDef;

using Statement = std::variant<IfStatement, FuncDef>;
using Statements = std::vector<Statement>;

class FuncDef {
   public:
    FuncDef(const std::string& name
            //, Token::Type returnType, std::vector<Param> params, Statements statements,
            // Position position
            )
        : name_{name} {}

    std::string_view getName() const { return name_; }

   private:
    std::string name_;
    Token::Type returnType_;
    std::vector<Param> params_;
    Statements statements_;
    Position position_;
};

#endif
