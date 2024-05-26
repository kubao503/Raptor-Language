#ifndef CALL_CONTEXT_H
#define CALL_CONTEXT_H

#include <vector>

#include "scope.hpp"

class CallContext {
   public:
    CallContext(const CallContext* parent)
        : parentContext_{parent}, scopes_{Scope()} {}

    void addVariable(VarEntry entry) { scopes_.back().addVariable(std::move(entry)); }

    void addFunction(const FuncDef* func) { scopes_.back().addFunction(func); }

    void addStruct(const StructDef* structDef) { scopes_.back().addStruct(structDef); }

    void addVariant(const VariantDef* variantDef) {
        scopes_.back().addVariant(variantDef);
    }

    void addScope() { scopes_.emplace_back(); }
    void removeScope() { scopes_.pop_back(); }

    std::optional<VarEntry> getVariable(std::string_view name) const;

    using FuncWithCtx = std::pair<const FuncDef*, const CallContext*>;
    std::optional<FuncWithCtx> getFunctionWithCtx(std::string_view name) const;
    const StructDef* getStructDef(std::string_view name) const;
    const VariantDef* getVariantDef(std::string_view name) const;

   private:
    const CallContext* parentContext_{nullptr};
    std::vector<Scope> scopes_;
};

#endif
