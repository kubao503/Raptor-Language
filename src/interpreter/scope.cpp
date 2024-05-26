#include "scope.hpp"

#include "interpreter_errors.hpp"

void Scope::addVariable(VarEntry entry) {
    if (getVariable(entry.name))
        throw VariableRedefinition{{}, entry.name};
    variables_.push_back(std::move(entry));
}

void Scope::addFunction(const FuncDef* func) {
    if (getFunction(func->getName()))
        throw FunctionRedefinition{{}, func->getName()};
    functions_.emplace_back(func);
}

void Scope::addStruct(const StructDef* structDef) {
    structs_.emplace_back(structDef);
}

void Scope::addVariant(const VariantDef* variantDef) {
    variants_.emplace_back(variantDef);
}

std::optional<RefObj> Scope::getVariable(std::string_view name) const {
    auto res = std::ranges::find(variables_, name, &VarEntry::name);
    if (res != variables_.end())
        return RefObj{.valueObj = res->valueObj.get(), .isConst = res->isConst};
    return std::nullopt;
}

std::optional<const FuncDef*> Scope::getFunction(std::string_view name) const {
    auto res = std::ranges::find(functions_, name, &FuncDef::getName);
    if (res != functions_.end())
        return *res;
    return std::nullopt;
}

Scope::StructDefEntry Scope::getStructDef(std::string_view name) const {
    auto res = std::ranges::find(structs_, name, &StructDef::name);
    if (res != structs_.end())
        return *res;
    return nullptr;
}

Scope::VariantDefEntry Scope::getVariantDef(std::string_view name) const {
    auto res = std::ranges::find(variants_, name, &VariantDef::name);
    if (res != variants_.end())
        return *res;
    return nullptr;
}
