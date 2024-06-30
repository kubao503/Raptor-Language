#ifndef INTERPRETER_SCOPE_H
#define INTERPRETER_SCOPE_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "parse_tree.hpp"
#include "types.hpp"
#include "value_obj.hpp"

namespace interpreter {
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
    const FuncDef* getFunction(std::string_view name) const;
    StructDefEntry getStructDef(std::string_view name) const;
    VariantDefEntry getVariantDef(std::string_view name) const;

   private:
    std::vector<VarEntry> variables_;
    std::vector<FuncDefEntry> functions_;
    std::vector<StructDefEntry> structs_;
    std::vector<VariantDefEntry> variants_;
};
}  // namespace interpreter

#endif
