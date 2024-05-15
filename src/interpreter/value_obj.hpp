#ifndef VALUE_OBJ_H
#define VALUE_OBJ_H

#include <memory>
#include <variant>
#include <vector>

#include "parse_tree.hpp"
#include "types.hpp"

struct ValueObj;

using ValueRef = std::shared_ptr<ValueObj>;

struct StructObj {
    std::vector<ValueRef> values;
};

struct ValueObj {
    using Value = std::variant<Integral, Floating, bool, std::string, StructObj>;

    ValueObj(Value value, Type type)
        : value{std::move(value)}, type{std::move(type)} {}

    Value value;
    Type type;
};

#endif
