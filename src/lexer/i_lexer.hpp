#ifndef I_LEXER_H
#define I_LEXER_H

#include "token.hpp"

class ILexer {
   public:
    virtual Token getToken() = 0;
    virtual ~ILexer() = default;
};

#endif
