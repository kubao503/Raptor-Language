#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <limits>
#include <string>

#include "position.hpp"
#include "types.hpp"

class LexerException : public std::exception {
   public:
    LexerException(const Position& position)
        : position_(position) {}

   protected:
    virtual std::string getName() const = 0;
    virtual std::string additionalInfo() const { return ""; }

   private:
    const Position position_;
    mutable std::string message_;

    const char* what() const noexcept override;
};

class InvalidToken : public LexerException {
   public:
    InvalidToken(const Position& position, char c)
        : LexerException(position), c_(c) {}

   private:
    const char c_;

    std::string getName() const override { return "InvalidToken"; }
    std::string additionalInfo() const override;
};

class InvalidNotEqualOp : public LexerException {
   public:
    using LexerException::LexerException;

   private:
    std::string getName() const override { return "InvalidNotEqualOp"; }
    std::string additionalInfo() const override;
};

class NotTerminatedStrConst : public LexerException {
   public:
    using LexerException::LexerException;

   private:
    std::string getName() const override { return "NotTerminatedStrConst"; }
    std::string additionalInfo() const override;
};

class NonEscapableChar : public LexerException {
   public:
    NonEscapableChar(const Position& position, char c)
        : LexerException(position), c_(c) {}

   private:
    const char c_;

    std::string getName() const override { return "NonEscapableChar"; }
    std::string additionalInfo() const override;
};

class NumericOverflow : public LexerException {
   public:
    NumericOverflow(const Position& position, integral_t value, integral_t digit)
        : LexerException(position), value_(value), digit_(digit) {}

   private:
    const integral_t value_;
    const integral_t digit_;

    std::string getName() const override { return "NumericOverflow"; }
    std::string additionalInfo() const override;
};

class InvalidFloat : public LexerException {
   public:
    using LexerException::LexerException;

   private:
    std::string getName() const override { return "InvalidFloat"; }
    std::string additionalInfo() const override;
};

#endif
