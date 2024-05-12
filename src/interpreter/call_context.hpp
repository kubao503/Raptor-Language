#ifndef CALL_CONTEXT_H
#define CALL_CONTEXT_H

#include <vector>

#include "scope.hpp"

class CallContext {
   public:
    CallContext()
        : scopes_{Scope()} {}

    void addVariable(const std::string& name, ValueRef ref) {
        scopes_.back().addVariable(name, std::move(ref));
    }

    Value readVariable(std::string_view name) const {
        using namespace std::literals::string_literals;

        for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter)
            if (auto var = scopes_.back().readVariable(name))
                return *var;
        throw std::runtime_error("Variable "s + std::string(name) + " not found");
    }

   private:
    std::vector<Scope> scopes_;
};

#endif
