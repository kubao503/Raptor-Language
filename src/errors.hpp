#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <limits>
#include <string>

#include "position.hpp"
#include "types.hpp"

class InvalidToken : public std::exception {
   public:
    InvalidToken(const Position& position)
        : position_(position) {}

   protected:
    virtual std::string getName() const { return "InvalidToken"; }
    virtual std::string additionalInfo() const { return ""; }

   private:
    const Position position_;
    mutable std::string message_;

    const char* what() const noexcept override;
};

class NotTerminatedStrConst : public InvalidToken {
   public:
    NotTerminatedStrConst(const Position& position, const std::string& strConst)
        : InvalidToken(position), strConst_(strConst) {}

   private:
    std::string getName() const override { return "NotTerminatedStrConst"; }
    std::string additionalInfo() const override;

    const std::string strConst_;
};

class NonEscapableChar : public InvalidToken {
   public:
    NonEscapableChar(const Position& position, char c)
        : InvalidToken(position), c_(c) {}

   private:
    const char c_;

    std::string getName() const override { return "NonEscapableChar"; }
    std::string additionalInfo() const override;
};

class IntOverflow : public InvalidToken {
   public:
    IntOverflow(const Position& position, integral_t value, integral_t digit)
        : InvalidToken(position), value_(value), digit_(digit) {}

   private:
    const integral_t value_;
    const integral_t digit_;

    std::string getName() const override { return "IntOverflow "; }
    std::string additionalInfo() const override;
};

class FloatOverflow : public std::exception {};

#endif
