#include "value_obj.hpp"

#include <algorithm>

#include "interpreter_errors.hpp"

NamedStructObj::NamedStructObj(Values values, const StructDef* structDef)
    : StructObj{std::move(values)}, structDef{structDef} {
    if (!structDef)
        throw std::runtime_error(
            "Cannot instantiate StructObj without struct definition");
}

ValueObj* NamedStructObj::getField(std::string_view fieldName) const {
    const auto field = std::ranges::find(structDef->fields, fieldName, &Field::name);
    if (field == structDef->fields.end())
        throw InvalidField{{}, fieldName};
    const auto index = std::ranges::distance(structDef->fields.begin(), field);
    return values.at(index).get();
}

struct ValueCopier {
    ValueObj::Value operator()(const StructObj& val) const {
        auto copiedValues = copyStructValues(val.values);
        return StructObj{std::move(copiedValues)};
    }

    ValueObj::Value operator()(const NamedStructObj& val) const {
        auto copiedValues = copyStructValues(val.values);
        return NamedStructObj{std::move(copiedValues), val.structDef};
    }

    ValueObj::Value operator()(const VariantObj& val) const {
        return VariantObj{
            std::make_unique<ValueObj>(std::visit(*this, val.valueObj->value)),
            val.variantDef};
    }

    ValueObj::Value operator()(const auto& v) const { return v; }

   private:
    StructObj::Values copyStructValues(const StructObj::Values& values) const {
        StructObj::Values copiedValues;
        const auto copyValue = [this](const std::unique_ptr<ValueObj>& val) {
            auto copiedValue = std::visit(*this, val->value);
            return std::make_unique<ValueObj>(std::move(copiedValue));
        };
        std::ranges::transform(values, std::back_inserter(copiedValues), copyValue);
        return copiedValues;
    }
};

ValueObj makeValueCopy(const ValueObj& obj) {
    return {std::visit(ValueCopier(), obj.value)};
}

struct GetHeldValue {
    ValueObj operator()(ValueObj obj) const { return obj; }
    ValueObj operator()(const RefObj& obj) const { return makeValueCopy(*obj.valueObj); }
};

ValueObj getHeldValue(ValueHolder holder) {
    return std::visit(GetHeldValue(), std::move(holder));
}

struct GetHeldValueCopy {
    ValueObj operator()(const ValueObj& obj) const { return makeValueCopy(obj); }
    ValueObj operator()(const RefObj& obj) const { return makeValueCopy(*obj.valueObj); }
};

ValueObj getHeldValueCopy(const ValueHolder& holder) {
    return std::visit(GetHeldValueCopy(), holder);
}
