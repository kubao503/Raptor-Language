#ifndef COMPILER_SCOPE_H
#define COMPILER_SCOPE_H

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace llvm {
class Value;
}

namespace compiler {
class Scope {
   public:
    using VarEntry = std::pair<std::string, llvm::Value*>;

    void addVariable(VarEntry varEntry) { varEntries_.push_back(std::move(varEntry)); }
    llvm::Value* getVariable(std::string_view name) const {
        auto res = std::ranges::find(varEntries_, name, &VarEntry::first);
        if (res == varEntries_.end())
            return nullptr;
        return res->second;
    }

   private:
    std::vector<VarEntry> varEntries_;
};
}  // namespace compiler

#endif  // SCOPE_H