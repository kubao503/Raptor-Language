#ifndef VALUE_OBJ_H
#define VALUE_OBJ_H

#include <memory>
#include <variant>
#include <vector>

#include "parse_tree.hpp"
#include "types.hpp"

struct ValueObj;

/// @brief Struct with just some unnamed values
struct StructObj {
    using Values = std::vector<std::unique_ptr<ValueObj>>;
    Values values;
};

/// @brief Struct with field names
struct NamedStructObj : public StructObj {
    NamedStructObj(Values values, const StructDef* structDef);
    ValueObj* getField(std::string_view fieldName) const;

    const StructDef* structDef;
};

struct VariantObj {
    std::unique_ptr<ValueObj> valueObj;
    const VariantDef* variantDef;
};

/// @brief Object owning a value
struct ValueObj {
    using Value = std::variant<Integral, Floating, bool, std::string, StructObj,
                               NamedStructObj, VariantObj>;
    Value value;
};

/// @brief Reference to ValueObj (non-owning)
struct RefObj {
    ValueObj* valueObj;
    bool isConst{false};
};

/// @brief Object from which a value can be obtained
using ValueHolder = std::variant<ValueObj, RefObj>;

/// @brief Returns the value of ValueHolder
/// @param holder
/// @return ValueObj with the same value as ValueHolder
ValueObj getHeldValue(ValueHolder holder);

/// @brief The same as getHeldValue() but ensures that the copy is made
/// @param holder
/// @return ValueObj with the same value as ValueHolder
ValueObj getHeldValueCopy(const ValueHolder& holder);

#endif
