#include "scope.hpp"

#include "interpreter_errors.hpp"

void Scope::addVariable(const std::string& name, ValueRef ref) {
    if (getVariable(name))
        throw VariableRedefinition{{}, name};
    variables_.emplace_back(name, std::move(ref));
}

void Scope::addFunction(const FuncDef* func) {
    if (getFunction(func->getName()))
        throw FunctionRedefinition{{}, func->getName()};
    functions_.emplace_back(func);
}

void Scope::addStruct(const StructDef* structDef) {
    if (getStructDef(structDef->name))
        throw StructRedefinition{{}, structDef->name};
    if (getVariantDef(structDef->name))
        throw VariantRedefinition{{}, structDef->name};
    structs_.emplace_back(structDef);
}

void Scope::addVariant(const VariantDef* variantDef) {
    if (getVariantDef(variantDef->name))
        throw VariantRedefinition{{}, variantDef->name};
    if (getStructDef(variantDef->name))
        throw StructRedefinition{{}, variantDef->name};
    variants_.emplace_back(variantDef);
}

std::optional<ValueRef> Scope::getVariable(std::string_view name) const {
    auto res = std::ranges::find(variables_, name, &VarEntry::first);
    if (res != variables_.end())
        return res->second;
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
