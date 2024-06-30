#include "call_context.hpp"

#include <algorithm>
#include <ranges>

namespace interpreter {
std::optional<RefObj> CallContext::getVariable(std::string_view name) const {
    for (const auto& scope : std::ranges::views::reverse(scopes_))
        if (const auto& varRef = scope.getVariable(name))
            return *varRef;

    const auto res = std::ranges::find(varRefs_, name, &RefEntry::first);
    if (res != varRefs_.end())
        return res->second;

    if (parentContext_)
        if (auto val = parentContext_->getVariable(name))
            return val;
    return std::nullopt;
}

std::optional<CallContext::FuncWithCtx> CallContext::getFunctionWithCtx(
    std::string_view name) const {
    for (auto const& scope : std::ranges::views::reverse(scopes_))
        if (const auto& func = scope.getFunction(name))
            return std::make_pair(func, this);

    if (parentContext_)
        if (auto funcWithCtx = parentContext_->getFunctionWithCtx(name))
            return funcWithCtx;
    return std::nullopt;
}

const StructDef* CallContext::getStructDef(std::string_view name) const {
    for (auto const& scope : std::ranges::views::reverse(scopes_))
        if (const auto& structDef = scope.getStructDef(name))
            return structDef;

    if (parentContext_)
        if (auto structDef = parentContext_->getStructDef(name))
            return structDef;

    return nullptr;
}

const VariantDef* CallContext::getVariantDef(std::string_view name) const {
    for (auto const& scope : std::ranges::views::reverse(scopes_))
        if (auto variantDef = scope.getVariantDef(name))
            return variantDef;

    if (parentContext_)
        if (auto variantDef = parentContext_->getVariantDef(name))
            return variantDef;

    return nullptr;
}
}  // namespace interpreter
