#ifndef CALL_CONTEXT_H
#define CALL_CONTEXT_H

#include <vector>

#include "scope.hpp"

class CallContext {
   public:
    CallContext(const CallContext* parent)
        : parentContext_{parent}, scopes_{Scope()} {}

    void addVariable(const std::string& name, ValueRef ref) {
        scopes_.back().addVariable(name, std::move(ref));
    }

    void addFunction(const FuncDef* func) { scopes_.back().addFunction(func); }

    void addStruct(const StructDef* structDef) { scopes_.back().addStruct(structDef); }

    void addVariant(const VariantDef* variantDef) {
        scopes_.back().addVariant(variantDef);
    }

    std::optional<ValueRef> getVariable(std::string_view name) const;

    using FuncWithCtx = std::pair<const FuncDef*, const CallContext*>;
    std::optional<FuncWithCtx> getFunctionWithCtx(std::string_view name) const;
    const StructDef* getStructDef(std::string_view name) const;
    const VariantDef* getVariantDef(std::string_view name) const;

   private:
    const CallContext* parentContext_{nullptr};
    std::vector<Scope> scopes_;
};

#endif
