#ifndef SCOPE_H
#define SCOPE_H

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "parse_tree.hpp"
#include "types.hpp"

struct ValueObj {
    ValueObj(Value value)
        : value{value} {}

    Value value;
};

using ValueRef = std::shared_ptr<ValueObj>;
using Symbol = std::pair<std::string, ValueRef>;

class Scope {
   public:
    void addVariable(const std::string& name, ValueRef ref) {
        variables_.emplace_back(name, ref);
    }

    void addFunction(const std::string& name, const FuncDef* func) {
        functions_.emplace_back(name, func);
    }

    std::optional<ValueRef> readVariable(std::string_view name) const {
        auto res = std::ranges::find(variables_, name, &Symbol::first);
        if (res != variables_.end())
            return res->second;
        return std::nullopt;
    }

    std::optional<const FuncDef*> getFunction(std::string_view name) const {
        auto res = std::ranges::find(functions_, name, &FunctionPair::first);
        if (res != functions_.end())
            return res->second;
        return std::nullopt;
    }

   private:
    std::vector<Symbol> variables_;
    using FunctionPair = std::pair<std::string, const FuncDef*>;
    std::vector<FunctionPair> functions_;
};

#endif
