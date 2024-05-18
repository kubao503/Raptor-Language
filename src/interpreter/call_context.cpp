#include "call_context.hpp"

std::optional<ValueRef> CallContext::getVariable(std::string_view name) const {
    for (auto iter = scopes_.crbegin(); iter != scopes_.crend(); ++iter)
        if (auto val = iter->getVariable(name))
            return val;

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

std::optional<const StructDef*> CallContext::getStructDef(std::string_view name) const {
    for (auto iter = scopes_.crbegin(); iter != scopes_.crend(); ++iter) {
        if (auto structDef = iter->getStructDef(name))
            return structDef;
    }

    if (parentContext_)
        if (auto structDef = parentContext_->getStructDef(name))
            return structDef;
    return std::nullopt;
}
