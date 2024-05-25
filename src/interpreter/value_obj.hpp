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

struct NamedStructObj : public StructObj {
    NamedStructObj(std::vector<ValueRef> values, const StructDef* structDef);
    ValueRef getField(std::string_view fieldName) const;

    const StructDef* structDef;
};

struct VariantObj {
    ValueRef valueRef;
    const VariantDef* variantDef;
};

struct ValueObj {
    using Value = std::variant<Integral, Floating, bool, std::string, StructObj,
                               NamedStructObj, VariantObj>;
    Value value;
};

#endif
