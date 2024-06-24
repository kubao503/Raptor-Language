#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "parse_tree.hpp"

class IRGenerator : public StatementVisitor {
   public:
    void genIR(const Program& program);

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
};

#endif  // IR_GENERATOR_H