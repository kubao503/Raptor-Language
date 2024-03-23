#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <limits>
#include <string>

#include "source.hpp"
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

    const char* what() const noexcept override {
        message_ = getName() + " at " + std::to_string(position_.line) + ':' +
                   std::to_string(position_.column) + '\n' + additionalInfo();

        return message_.c_str();
    };
};

class NotTerminatedStrConst : public InvalidToken {
   public:
    NotTerminatedStrConst(const Position& position, const std::string& strConst)
        : InvalidToken(position), strConst_(strConst) {}

   private:
    std::string getName() const override { return "NotTerminatedStrConst"; }
    std::string additionalInfo() const override { return "str: " + strConst_; }

    const std::string strConst_;
};

class NonEscapableChar : public InvalidToken {
   public:
    NonEscapableChar(const Position& position, char c)
        : InvalidToken(position), c_(c) {}

   private:
    const char c_;

    std::string getName() const override { return "NonEscapableChar"; }
    std::string additionalInfo() const override { return "char: '" + std::string(1, c_) + '\''; }
};

class IntOverflow : public InvalidToken {
   public:
    IntOverflow(const Position& position, integral_t value, integral_t digit)
        : InvalidToken(position), value_(value), digit_(digit) {}

   private:
    const integral_t value_;
    const integral_t digit_;

    std::string getName() const override { return "IntOverflow "; }
    std::string additionalInfo() const override {
        auto max = std::numeric_limits<integral_t>::max();
        return std::to_string(value_) + " * 10 + " + std::to_string(digit_) + " > " +
               std::to_string(max);
    }
};

class FloatOverflow : public std::exception {};

#endif
