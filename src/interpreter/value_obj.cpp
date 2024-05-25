#include "value_obj.hpp"

#include <algorithm>

#include "interpreter_errors.hpp"

NamedStructObj::NamedStructObj(std::vector<ValueRef> values, const StructDef* structDef)
    : StructObj{std::move(values)}, structDef{structDef} {
    if (!structDef)
        throw std::runtime_error(
            "Cannot instantiate StructObj without struct definition");
}

ValueRef NamedStructObj::getField(std::string_view fieldName) const {
    const auto field = std::ranges::find(structDef->fields, fieldName, &Field::name);
    if (field == structDef->fields.end())
        throw InvalidField{{}, fieldName};
    const auto index = std::ranges::distance(structDef->fields.begin(), field);
    return values.at(index);
}
