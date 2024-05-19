#ifndef INTERPRETER_ERRORS_H
#define INTERPRETER_ERRORS_H

#include "base_errors.hpp"
#include "types.hpp"

class SymbolNotFound : public BaseException {
   public:
    SymbolNotFound(const Position& position, std::string type, std::string symbol);
    SymbolNotFound(const Position& position, SymbolNotFound e);

   private:
    std::string type_;
    std::string symbol_;
};

class TypeMismatch : public BaseException {
   public:
    TypeMismatch(const Position& position, Type expected, Type actual);
    TypeMismatch(const Position& position, TypeMismatch e);

   private:
    Type expected_;
    Type actual_;
};

#endif
