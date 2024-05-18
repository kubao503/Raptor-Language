#include "call_context.hpp"

std::optional<ValueRef> CallContext::readVariable(std::string_view name) const {
    for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter)
        if (auto val = iter->readVariable(name))
            return val;

    if (parentContext_)
        if (auto val = parentContext_->readVariable(name))
            return val;

    throw std::runtime_error("Variable " + std::string(name) + " not found");
}

std::optional<CallContext::FuncWithCtx> CallContext::getFunctionWithCtx(
    std::string_view name) const {
    for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter)
        if (auto func = iter->getFunction(name)) {
            return std::make_pair(*func, this);
        }

    if (parentContext_)
        if (auto funcWithCtx = parentContext_->getFunctionWithCtx(name))
            return funcWithCtx;

    throw std::runtime_error("Function " + std::string(name) + " not found");
}

std::optional<const StructDef*> CallContext::getStructDef(std::string_view name) const {
    for (auto iter = scopes_.crbegin(); iter != scopes_.crend(); ++iter) {
        if (auto structDef = iter->getStructDef(name))
            return structDef;
    }

    if (parentContext_)
        if (auto structDef = parentContext_->getStructDef(name))
            return structDef;

    throw std::runtime_error("Struct definition " + std::string(name) + " not found");
}
