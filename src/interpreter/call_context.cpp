#include "call_context.hpp"

std::optional<RefObj> CallContext::getVariable(std::string_view name) const {
    for (auto iter = scopes_.crbegin(); iter != scopes_.crend(); ++iter)
        if (auto val = iter->getVariable(name))
            return *val;

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
    for (auto iter = scopes_.crbegin(); iter != scopes_.crend(); ++iter)
        if (auto func = iter->getFunction(name)) {
            return std::make_pair(*func, this);
        }

    if (parentContext_)
        if (auto funcWithCtx = parentContext_->getFunctionWithCtx(name))
            return funcWithCtx;
    return std::nullopt;
}

const StructDef* CallContext::getStructDef(std::string_view name) const {
    for (auto iter = scopes_.crbegin(); iter != scopes_.crend(); ++iter) {
        if (auto structDef = iter->getStructDef(name))
            return structDef;
    }

    if (parentContext_)
        if (auto structDef = parentContext_->getStructDef(name))
            return structDef;

    return nullptr;
}

const VariantDef* CallContext::getVariantDef(std::string_view name) const {
    for (auto iter = scopes_.crbegin(); iter != scopes_.crend(); ++iter) {
        if (auto variantDef = iter->getVariantDef(name))
            return variantDef;
    }

    if (parentContext_)
        if (auto variantDef = parentContext_->getVariantDef(name))
            return variantDef;

    return nullptr;
}
