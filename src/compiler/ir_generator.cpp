#include "ir_generator.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#pragma GCC diagnostic pop

#include "print.hpp"
#include "semantic_errors.hpp"

constexpr const char* printFuncName{"printValue"};

namespace compiler {
IRGenerator::IRGenerator()
    : exprIRGenerator_(this) {
    InitializeModule();
    addPrintfFunc();
    createMainFunc();
    scopeStack_.emplace();
}

void IRGenerator::InitializeModule() {
    context_ = std::make_unique<llvm::LLVMContext>();
    module_ = std::make_unique<llvm::Module>("The Module", *context_);
    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
}

void IRGenerator::addPrintfFunc() const {
    const std::vector<llvm::Type*> paramTypes{llvm::Type::getInt32Ty(*context_)};
    const auto funcType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), paramTypes, true);

    llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, printFuncName,
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

void IRGenerator::addVariable(Scope::VarEntry varEntry) {
    scopeStack_.top().addVariable(std::move(varEntry));
}

llvm::Value* IRGenerator::getVariable(std::string_view name) const {
    return scopeStack_.top().getVariable(name);
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

TypeIndex getTypeID(llvm::Value* value) {
    const auto type = value->getType()->getTypeID();
    switch (type) {
        case llvm::Type::IntegerTyID: {
            if (value->getType()->getIntegerBitWidth() == 1)
                return TypeIndex::BOOL;
            return TypeIndex::INT;
        }
        case llvm::Type::DoubleTyID:
            return TypeIndex::FLOAT;
        case llvm::Type::PointerTyID:
            return TypeIndex::STR;
        default:
            throw std::runtime_error("Unknown type id" + std::to_string(type));
    }
}

void IRGenerator::operator()(const PrintStatement& stmt) {
    const auto func = module_->getFunction(printFuncName);
    assert(func && printFuncName && " not found");

    std::vector<llvm::Value*> args;

    if (auto expr = stmt.expression.get()) {
        const auto value = getIRFromExpr(*expr);
        const auto typeId = static_cast<int>(getTypeID(value));
        args.push_back(llvm::ConstantInt::get(builder_->getInt32Ty(), typeId));
        args.push_back(value);
    } else {
        args.push_back(llvm::ConstantInt::get(builder_->getInt32Ty(),
                                              static_cast<int>(TypeIndex::VOID)));
    }

    builder_->CreateCall(func, args, "calltmp");
}

void IRGenerator::operator()(const FuncDef&) {}
void IRGenerator::operator()(const Assignment&) {}

void IRGenerator::operator()(const VarDef& stmt) {
    if (getVariable(stmt.name))
        throw VariableRedefinition(stmt.position, stmt.name);
    auto initValue = getIRFromExpr(*stmt.expression);

    auto alloca = createEntryBlockAlloca(stmt.name);
    builder_->CreateStore(initValue, alloca);
    addVariable({stmt.name, alloca});
}

void IRGenerator::operator()(const FuncCall&) {}
void IRGenerator::operator()(const StructDef&) {}
void IRGenerator::operator()(const VariantDef&) {}

llvm::Value* IRGenerator::getIRFromExpr(const Expression& expr) {
    expr.accept(exprIRGenerator_);
    const auto value = exprIRGenerator_.getLastValue();
    assert(value && "Expression value not set");
    return value;
}

llvm::AllocaInst* IRGenerator::createEntryBlockAlloca(std::string_view name) {
    auto func = builder_->GetInsertBlock()->getParent();
    llvm::IRBuilder<> TmpB(&func->getEntryBlock(), func->getEntryBlock().begin());
    return TmpB.CreateAlloca(llvm::Type::getInt32Ty(*context_), nullptr, name);
}
}  // namespace compiler