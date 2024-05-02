#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <variant>

using Integral = unsigned int;
using Floating = float;

using Value = std::variant<std::monostate, Integral, Floating, bool, std::string>;

#endif
