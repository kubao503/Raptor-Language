#include "ir_generator.hpp"

void IRGenerator::genIR(const Program& program) {
    for (const auto& stmt : program.statements)
        stmt->accept(*this);
}

void IRGenerator::operator()(const IfStatement&) {}
void IRGenerator::operator()(const WhileStatement&) {}
void IRGenerator::operator()(const ReturnStatement&) {}
void IRGenerator::operator()(const PrintStatement&) {}
void IRGenerator::operator()(const FuncDef&) {}
void IRGenerator::operator()(const Assignment&) {}
void IRGenerator::operator()(const VarDef&) {}
void IRGenerator::operator()(const FuncCall&) {}
void IRGenerator::operator()(const StructDef&) {}
void IRGenerator::operator()(const VariantDef&) {}