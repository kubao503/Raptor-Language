#ifndef PARSER_ERRORS_H
#define PARSER_ERRORS_H

#include "base_errors.hpp"

class SyntaxException : public BaseException {
   public:
    SyntaxException(const Position& position, const std::string& message)
        : BaseException(position, message) {}
};

#endif
