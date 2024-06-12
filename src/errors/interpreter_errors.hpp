#ifndef INTERPRETER_ERRORS_H
#define INTERPRETER_ERRORS_H

#include "base_errors.hpp"
#include "types.hpp"
#include "value_obj.hpp"

class SymbolNotFound : public BaseException {
   public:
    SymbolNotFound(const Position& position, std::string type, std::string symbol);
    SymbolNotFound(const Position& position, const SymbolNotFound& e);

   private:
    std::string type_;
    std::string symbol_;
};

class TypeMismatch : public BaseException {
   public:
    TypeMismatch(const Position& position, Type expected, Type actual);
    TypeMismatch(const Position& position, const TypeMismatch& e);

   private:
    Type expected_;
    Type actual_;
};

class ReturnTypeMismatch : public BaseException {
   public:
    ReturnTypeMismatch(const Position& position, ReturnType expected, ReturnType actual);
    ReturnTypeMismatch(const Position& position, const ReturnTypeMismatch& e);

   private:
    ReturnType expected_;
    ReturnType actual_;
};

class InvalidFieldCount : public BaseException {
   public:
    InvalidFieldCount(const Position& position, std::size_t expected, std::size_t actual);
    InvalidFieldCount(const Position& position, const InvalidFieldCount& e);

   private:
    std::size_t expected_;
    std::size_t actual_;
};

class InvalidField : public BaseException {
   public:
    InvalidField(const Position& position, std::string_view fieldName);
    InvalidField(const Position& position, const InvalidField& e);

   private:
    std::string fieldName_;
};

class Redefinition : public BaseException {
   protected:
    Redefinition(const Position& position, std::string type, std::string name);

    std::string name_;
};

class VariableRedefinition : public Redefinition {
   public:
    VariableRedefinition(const Position& position, std::string name)
        : Redefinition{position, "variable", std::move(name)} {}
    VariableRedefinition(const Position& position, const VariableRedefinition& e)
        : VariableRedefinition{position, e.name_} {}
};

class FunctionRedefinition : public Redefinition {
   public:
    FunctionRedefinition(const Position& position, std::string name)
        : Redefinition{position, "function", std::move(name)} {}
    FunctionRedefinition(const Position& position, const FunctionRedefinition& e)
        : FunctionRedefinition{position, e.name_} {}
};

class StructRedefinition : public Redefinition {
   public:
    StructRedefinition(const Position& position, std::string name)
        : Redefinition{position, "struct", std::move(name)} {}
    StructRedefinition(const Position& position, const StructRedefinition& e)
        : StructRedefinition{position, e.name_} {}
};

class VariantRedefinition : public Redefinition {
   public:
    VariantRedefinition(const Position& position, std::string name)
        : Redefinition{position, "variant", std::move(name)} {}
    VariantRedefinition(const Position& position, const VariantRedefinition& e)
        : VariantRedefinition{position, e.name_} {}
};

class InvalidTypeConversion : public BaseException {
   public:
    InvalidTypeConversion(const Position& position, ValueObj::Value from, Type to);
    InvalidTypeConversion(const Position& position, InvalidTypeConversion e);

   private:
    ValueObj::Value from_;
    Type to_;
};

class ConstViolation : public BaseException {
   public:
    explicit ConstViolation(const Position& position)
        : BaseException{position, "Violation of const constraint"} {}
};

class MaxRecursionDepth : public BaseException {
   public:
    MaxRecursionDepth(const Position& position)
        : BaseException{position, "Max recursion depth reached"} {}
};

class DivisionByZero : public BaseException {
   public:
    DivisionByZero(const Position& position)
        : BaseException{position, "Attempt to divide by zero"} {}
};

#endif
