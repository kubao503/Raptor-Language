#include "call_context.hpp"

std::optional<Value> CallContext::readVariable(std::string_view name) const {
    using namespace std::literals::string_literals;

    for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter)
        if (auto val = iter->readVariable(name))
            return val;

    if (parentContext_)
        if (auto val = parentContext_->readVariable(name))
            return val;

    throw std::runtime_error("Variable "s + std::string(name) + " not found");
}

std::optional<CallContext::FuncWithCtx> CallContext::getFunctionWithCtx(
    std::string_view name) const {
    using namespace std::literals::string_literals;

    for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter)
        if (auto func = iter->getFunction(name)) {
            return std::make_pair(*func, this);
        }

    if (parentContext_)
        if (auto funcWithCtx = parentContext_->getFunctionWithCtx(name))
            return funcWithCtx;

    throw std::runtime_error("Function "s + std::string(name) + " not found");
}
