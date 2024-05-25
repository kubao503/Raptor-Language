#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <variant>

using Integral = int;
using Floating = float;

enum class BuiltInType {
    INT,
    FLOAT,
    BOOL,
    STR,
};

struct VoidType {};

using Type = std::variant<std::string, BuiltInType>;
using ReturnType = std::variant<std::string, BuiltInType, VoidType>;

#endif
