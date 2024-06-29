#ifndef LEXER_ERRORS_H
#define LEXER_ERRORS_H

#include "base_exception.hpp"
#include "types.hpp"

class InvalidToken : public BaseException {
   public:
    InvalidToken(const Position& position, char c);
};

class NotTerminatedStrConst : public BaseException {
   public:
    explicit NotTerminatedStrConst(const Position& position);
};

class NonEscapableChar : public BaseException {
   public:
    NonEscapableChar(const Position& position, char c);
};

class NumericOverflow : public BaseException {
   public:
    NumericOverflow(const Position& position, Integral value, Integral digit);
};

class InvalidFloat : public BaseException {
   public:
    explicit InvalidFloat(const Position& position);
};

#endif
