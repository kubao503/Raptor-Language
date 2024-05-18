#ifndef SCOPE_H
#define SCOPE_H

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "parse_tree.hpp"
#include "types.hpp"
#include "value_obj.hpp"

class Scope {
    using VarEntry = std::pair<std::string, ValueRef>;
    using FuncDefEntry = const FuncDef*;
    using StructDefEntry = const StructDef*;

   public:
    void addVariable(const std::string& name, ValueRef ref) {
        variables_.emplace_back(name, std::move(ref));
    }

    void addFunction(const FuncDef* func) { functions_.emplace_back(func); }

    void addStruct(const StructDef* structDef) { structs_.emplace_back(structDef); }

    std::optional<ValueRef> readVariable(std::string_view name) const {
        auto res = std::ranges::find(variables_, name, &VarEntry::first);
        if (res != variables_.end())
            return res->second;
        return std::nullopt;
    }

    std::optional<const FuncDef*> getFunction(std::string_view name) const {
        auto res = std::ranges::find(functions_, name, &FuncDef::getName);
        if (res != functions_.end())
            return *res;
        return std::nullopt;
    }

    std::optional<StructDefEntry> getStructDef(std::string_view name) const {
        auto res = std::ranges::find(structs_, name, &StructDef::name);
        if (res != structs_.end())
            return *res;
        return std::nullopt;
    }

   private:
    std::vector<VarEntry> variables_;
    std::vector<FuncDefEntry> functions_;
    std::vector<StructDefEntry> structs_;
};

#endif
