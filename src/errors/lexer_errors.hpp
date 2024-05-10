#ifndef LEXER_ERRORS_H
#define LEXER_ERRORS_H

#include "base_errors.hpp"
#include "types.hpp"

class InvalidToken : public BaseException {
   public:
    InvalidToken(const Position& position, char c);

   private:
    char c_;
};

class NotTerminatedStrConst : public BaseException {
   public:
    explicit NotTerminatedStrConst(const Position& position);
};

class NonEscapableChar : public BaseException {
   public:
    NonEscapableChar(const Position& position, char c);

   private:
    char c_;
};

class NumericOverflow : public BaseException {
   public:
    NumericOverflow(const Position& position, Integral value, Integral digit);

   private:
    Integral value_;
    Integral digit_;
};

class InvalidFloat : public BaseException {
   public:
    explicit InvalidFloat(const Position& position);
};

#endif
