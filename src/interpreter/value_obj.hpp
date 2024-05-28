#ifndef VALUE_OBJ_H
#define VALUE_OBJ_H

#include <memory>
#include <variant>
#include <vector>

#include "parse_tree.hpp"
#include "types.hpp"

struct ValueObj;

struct StructObj {
    using Values = std::vector<std::unique_ptr<ValueObj>>;
    Values values;
};

struct NamedStructObj : public StructObj {
    NamedStructObj(Values values, const StructDef* structDef);
    ValueObj* getField(std::string_view fieldName) const;

    const StructDef* structDef;
};

struct VariantObj {
    std::unique_ptr<ValueObj> valueObj;
    const VariantDef* variantDef;
};

struct ValueObj {
    using Value = std::variant<Integral, Floating, bool, std::string, StructObj,
                               NamedStructObj, VariantObj>;
    Value value;
};

struct RefObj {
    ValueObj* valueObj;
    bool isConst{false};
};

using ValueHolder = std::variant<ValueObj, RefObj>;

ValueObj getHeldValue(ValueHolder holder);
ValueObj getHeldValueCopy(const ValueHolder& holder);

#endif
