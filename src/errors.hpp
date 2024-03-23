#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <string>

#include "source.hpp"

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

    std::string strConst_;
};

class NonEscapableChar : public InvalidToken {
   public:
    NonEscapableChar(const Position& position, char c)
        : InvalidToken(position), c_(c) {}

   private:
    std::string getName() const override { return "NonEscapableChar"; }
    std::string additionalInfo() const override { return "char: '" + std::string(1, c_) + '\''; }

    char c_;
};

class IntOverflow : public std::exception {};

class FloatOverflow : public std::exception {};

#endif
