#ifndef INTERPRETER_ERRORS_H
#define INTERPRETER_ERRORS_H

#include "base_errors.hpp"

class SymbolNotFound : public BaseException {
   public:
    SymbolNotFound(const Position& position, std::string type, std::string symbol)
        : BaseException{position, type + " " + symbol + " not found"} {}
};

#endif
