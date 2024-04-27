#ifndef ERRORS_H
#define ERRORS_H

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "position.hpp"
#include "types.hpp"

/// @brief Abstract class for exceptions thrown by Lexer
class LexerException : public std::runtime_error {
   public:
    explicit LexerException(const Position& position, const std::string& message);

    /// @brief Provides std::string representation of exception
    /// @return std::string representation of exception
    std::string describe() const;

   private:
    Position position_;

    std::string getName() const;
    using std::runtime_error::what;
};

class InvalidToken : public LexerException {
   public:
    InvalidToken(const Position& position, char c);

   private:
    char c_;
};

class NotTerminatedStrConst : public LexerException {
   public:
    explicit NotTerminatedStrConst(const Position& position);
};

class NonEscapableChar : public LexerException {
   public:
    NonEscapableChar(const Position& position, char c);

   private:
    char c_;
};

class NumericOverflow : public LexerException {
   public:
    NumericOverflow(const Position& position, Integral value, Integral digit);

   private:
    Integral value_;
    Integral digit_;
};

class InvalidFloat : public LexerException {
   public:
    explicit InvalidFloat(const Position& position);
};

#endif
