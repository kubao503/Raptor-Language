#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <variant>
#include <vector>

struct IfStatement {};

using Statement = std::variant<IfStatement>;
using Statements = std::vector<Statement>;

#endif
