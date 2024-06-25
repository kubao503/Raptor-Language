#include "ExprIRGenerator.hpp"

#include "IRGenerator.hpp"
#include "llvm/IR/Constants.h"

void ExprIRGenerator::operator()(const StructInitExpression&) const {}
void ExprIRGenerator::operator()(const DisjunctionExpression&) const {}
void ExprIRGenerator::operator()(const ConjunctionExpression&) const {}
void ExprIRGenerator::operator()(const EqualExpression&) const {}
void ExprIRGenerator::operator()(const NotEqualExpression&) const {}
void ExprIRGenerator::operator()(const LessThanExpression&) const {}
void ExprIRGenerator::operator()(const LessThanOrEqualExpression&) const {}
void ExprIRGenerator::operator()(const GreaterThanExpression&) const {}
void ExprIRGenerator::operator()(const GreaterThanOrEqualExpression&) const {}
void ExprIRGenerator::operator()(const AdditionExpression&) const {}
void ExprIRGenerator::operator()(const SubtractionExpression&) const {}
void ExprIRGenerator::operator()(const MultiplicationExpression&) const {}
void ExprIRGenerator::operator()(const DivisionExpression&) const {}
void ExprIRGenerator::operator()(const SignChangeExpression&) const {}
void ExprIRGenerator::operator()(const LogicalNegationExpression&) const {}
void ExprIRGenerator::operator()(const ConversionExpression&) const {}
void ExprIRGenerator::operator()(const TypeCheckExpression&) const {}
void ExprIRGenerator::operator()(const FieldAccessExpression&) const {}

void ExprIRGenerator::operator()(const Constant& expr) const {
    auto constantToValue = overloaded{
        [this](int v) {
            return static_cast<llvm::Value*>(
                llvm::ConstantInt::get(irGenerator_->builder_->getInt32Ty(), v));
        },
        [this](float v) {
            return static_cast<llvm::Value*>(
                llvm::ConstantFP::get(irGenerator_->builder_->getDoubleTy(), v));
        },
        [this](bool v) {
            return static_cast<llvm::Value*>(
                llvm::ConstantInt::get(irGenerator_->builder_->getInt1Ty(), v));
        },
        [this](const std::string& v) {
            const auto str =
                llvm::ConstantDataArray::getString(*irGenerator_->context_, v);

            const auto strVar =
                new llvm::GlobalVariable(*irGenerator_->module_, str->getType(), true,
                                         llvm::GlobalValue::PrivateLinkage, str, ".str");

            llvm::Constant* zero = llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(*irGenerator_->context_), 0);
            llvm::Constant* indices[] = {zero, zero};
            llvm::Constant* strPtr =
                llvm::ConstantExpr::getGetElementPtr(str->getType(), strVar, indices);

            return static_cast<llvm::Value*>(strPtr);
        }};

    lastValue_ = std::visit(constantToValue, expr.value);
}

void ExprIRGenerator::operator()(const FuncCall&) const {}
void ExprIRGenerator::operator()(const VariableAccess&) const {}
