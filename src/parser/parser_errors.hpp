#ifndef PARSER_ERRORS_H
#define PARSER_ERRORS_H

#include "base_exception.hpp"

class SyntaxException : public BaseException {
   public:
    using BaseException::BaseException;
};

class NoTypesInVariant : public BaseException {
   public:
    explicit NoTypesInVariant(const Position& position)
        : BaseException{
              position,
              "Variant record requires at least one type but none were specified"} {}
};

#endif
