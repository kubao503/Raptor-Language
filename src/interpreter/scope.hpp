#ifndef SCOPE_H
#define SCOPE_H

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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
        variables_.push_back({name, ref});
    }

    std::optional<Value> readVariable(std::string_view name) const {
        auto res = std::ranges::find(variables_, name, &Symbol::first);
        if (res != variables_.end())
            return res->second->value;
        return std::nullopt;
    }

   private:
    std::vector<Symbol> variables_;
};

#endif
