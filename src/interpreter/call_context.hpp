#ifndef CALL_CONTEXT_H
#define CALL_CONTEXT_H

#include <vector>

#include "scope.hpp"

class CallContext {
   public:
    using RefEntry = std::pair<std::string, RefObj>;
    using FuncWithCtx = std::pair<const FuncDef*, const CallContext*>;

    explicit CallContext(const CallContext* parent)
        : parentContext_{parent} {
        addScope();
    }

    void addVariable(VarEntry entry) { scopes_.back().addVariable(std::move(entry)); }
    void addReference(RefEntry entry) { varRefs_.push_back(std::move(entry)); }
    void addFunction(const FuncDef* func) { scopes_.back().addFunction(func); }
    void addStruct(const StructDef* structDef) { scopes_.back().addStruct(structDef); }
    void addVariant(const VariantDef* variantDef) {
        scopes_.back().addVariant(variantDef);
    }

    void addScope() { scopes_.emplace_back(); }
    void removeScope() { scopes_.pop_back(); }

    std::optional<RefObj> getVariable(std::string_view name) const;
    std::optional<FuncWithCtx> getFunctionWithCtx(std::string_view name) const;
    const StructDef* getStructDef(std::string_view name) const;
    const VariantDef* getVariantDef(std::string_view name) const;

   private:
    const CallContext* parentContext_{nullptr};
    std::vector<Scope> scopes_;
    std::vector<RefEntry> varRefs_;
};

#endif
