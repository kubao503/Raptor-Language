#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include <stack>

#include "expr_ir_generator.hpp"
#include "parse_tree.hpp"
#include "scope.hpp"

namespace llvm {
class LLVMContext;
class Module;
}  // namespace llvm

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "llvm/IR/IRBuilder.h"

#pragma GCC diagnostic pop

namespace compiler {
class IRGenerator : public StatementVisitor {
   public:
    IRGenerator();
    void genIR(const Program& program);
    auto takeModule() { return std::move(module_); }

    void addVariable(Scope::VarEntry varEntry);
    llvm::Value* getVariable(std::string_view name) const;

   private:
    void operator()(const IfStatement& stmt) override;
    void operator()(const WhileStatement& stmt) override;
    void operator()(const ReturnStatement& stmt) override;
    void operator()(const PrintStatement& stmt) override;
    void operator()(const FuncDef& stmt) override;
    void operator()(const Assignment& stmt) override;
    void operator()(const VarDef& stmt) override;
    void operator()(const FuncCall& stmt) override;
    void operator()(const StructDef& stmt) override;
    void operator()(const VariantDef& stmt) override;

    void InitializeModule();
    void addPrintfFunc() const;
    void createMainFunc() const;

    llvm::Value* getIRFromExpr(const Expression& expr);
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Type* type,
                                             std::string_view name) const;

    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;

    ExprIRGenerator exprIRGenerator_;
    std::stack<Scope> scopeStack_;

    friend class ExprIRGenerator;
};
}  // namespace compiler

#endif  // IR_GENERATOR_H