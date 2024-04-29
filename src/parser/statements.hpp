#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <variant>
#include <vector>

struct IfStatement {};
struct FuncDef {};

using Statement = std::variant<IfStatement, FuncDef>;
using Statements = std::vector<Statement>;

#endif
