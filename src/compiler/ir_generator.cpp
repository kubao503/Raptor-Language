#include "ir_generator.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#pragma GCC diagnostic pop

IRGenerator::IRGenerator() {
    InitializeModule();
    addPrintfFunc();
    createMainFunc();
}

void IRGenerator::InitializeModule() {
    context_ = std::make_unique<llvm::LLVMContext>();
    module_ = std::make_unique<llvm::Module>("The Module", *context_);
    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
}

void IRGenerator::addPrintfFunc() const {
    const std::vector<llvm::Type*> paramTypes{
        llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    const auto funcType =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), paramTypes, true);

    llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "printf",
                           module_.get());
}

void IRGenerator::createMainFunc() const {
    const std::vector<llvm::Type*> paramTypes;
    const auto funcType =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), paramTypes, false);

    const auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                             "main", module_.get());

    const auto block = llvm::BasicBlock::Create(*context_, "entry", func);
    builder_->SetInsertPoint(block);
}

void IRGenerator::genIR(const Program& program) {
    for (const auto& stmt : program.statements)
        stmt->accept(*this);

    builder_->CreateRet(llvm::ConstantInt::get(builder_->getInt32Ty(), 0));

    const auto mainFunc = module_->getFunction("main");
    llvm::verifyFunction(*mainFunc);

    module_->print(llvm::outs(), nullptr);
}

void IRGenerator::operator()(const IfStatement&) {}
void IRGenerator::operator()(const WhileStatement&) {}
void IRGenerator::operator()(const ReturnStatement&) {}

void IRGenerator::operator()(const PrintStatement&) {
    auto func = module_->getFunction("printf");
    assert(func && "printf not found");

    const auto formatStr = builder_->CreateGlobalStringPtr("%d\n");
    const auto val = llvm::ConstantInt::get(builder_->getInt32Ty(), 42);

    const std::vector<llvm::Value*> args{formatStr, val};

    builder_->CreateCall(func, args, "calltmp");
}

void IRGenerator::operator()(const FuncDef&) {}
void IRGenerator::operator()(const Assignment&) {}
void IRGenerator::operator()(const VarDef&) {}
void IRGenerator::operator()(const FuncCall&) {}
void IRGenerator::operator()(const StructDef&) {}
void IRGenerator::operator()(const VariantDef&) {}