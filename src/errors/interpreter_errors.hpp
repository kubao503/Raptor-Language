#ifndef INTERPRETER_ERRORS_H
#define INTERPRETER_ERRORS_H

#include "base_errors.hpp"
#include "types.hpp"

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

#endif
