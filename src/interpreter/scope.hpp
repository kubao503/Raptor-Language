#ifndef SCOPE_H
#define SCOPE_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "parse_tree.hpp"
#include "types.hpp"
#include "value_obj.hpp"

struct VarEntry {
    std::string name;
    std::unique_ptr<ValueObj> valueObj;
    bool isConst{false};
};

class Scope {
    using FuncDefEntry = const FuncDef*;
    using StructDefEntry = const StructDef*;
    using VariantDefEntry = const VariantDef*;

   public:
    void addVariable(VarEntry entry);
    void addFunction(const FuncDef* func);
    void addStruct(const StructDef* structDef);
    void addVariant(const VariantDef* variantDef);

    std::optional<RefObj> getVariable(std::string_view name) const;
    std::optional<const FuncDef*> getFunction(std::string_view name) const;
    StructDefEntry getStructDef(std::string_view name) const;
    VariantDefEntry getVariantDef(std::string_view name) const;

   private:
    std::vector<VarEntry> variables_;
    std::vector<FuncDefEntry> functions_;
    std::vector<StructDefEntry> structs_;
    std::vector<VariantDefEntry> variants_;
};

#endif
