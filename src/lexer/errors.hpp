#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <limits>
#include <string>

#include "position.hpp"
#include "types.hpp"

class LexerException : public std::exception {
   public:
    explicit LexerException(const Position& position)
        : position_(position) {}

   protected:
    virtual std::string additionalInfo() const { return ""; }

   private:
    Position position_;
    mutable std::string message_;

    std::string getName() const;
    const char* what() const noexcept override;
};

class InvalidToken : public LexerException {
   public:
    InvalidToken(const Position& position, char c)
        : LexerException(position), c_(c) {}

   private:
    char c_;

    std::string additionalInfo() const override;
};

class NotTerminatedStrConst : public LexerException {
   public:
    using LexerException::LexerException;

   private:
    std::string additionalInfo() const override;
};

class NonEscapableChar : public LexerException {
   public:
    NonEscapableChar(const Position& position, char c)
        : LexerException(position), c_(c) {}

   private:
    char c_;

    std::string additionalInfo() const override;
};

class NumericOverflow : public LexerException {
   public:
    NumericOverflow(const Position& position, Integral value, Integral digit)
        : LexerException(position), value_(value), digit_(digit) {}

   private:
    Integral value_;
    Integral digit_;

    std::string additionalInfo() const override;
};

class InvalidFloat : public LexerException {
   public:
    using LexerException::LexerException;

   private:
    std::string additionalInfo() const override;
};

#endif
