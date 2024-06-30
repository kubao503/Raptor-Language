#ifndef CALL_CONTEXT_H
#define CALL_CONTEXT_H

#include <vector>

#include "scope.hpp"

namespace interpreter {
using RefEntry = std::pair<std::string, RefObj>;

/// @brief Function call context
class CallContext {
   public:
    using FuncWithCtx = std::pair<const FuncDef*, const CallContext*>;

    /// @param parent The context in which the function is called or nullptr if this is a
    /// global context
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

    /// @brief Returns a function with the given name along with the call context in which
    /// the function is defined
    /// @param name Named of the function
    /// @return
    std::optional<FuncWithCtx> getFunctionWithCtx(std::string_view name) const;
    const StructDef* getStructDef(std::string_view name) const;
    const VariantDef* getVariantDef(std::string_view name) const;

   private:
    const CallContext* parentContext_{nullptr};
    std::vector<Scope> scopes_;
    std::vector<RefEntry> varRefs_;
};
}  // namespace interpreter

#endif
