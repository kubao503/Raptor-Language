#ifndef FILTER_H
#define FILTER_H

#include "ILexer.hpp"

class Filter : public ILexer {
   public:
    Filter(ILexer &lexer, Token::Type ignoring)
        : lexer_(lexer), ignoring_(ignoring) {}

    Token getToken() override {
        Token token;

        do {
            token = lexer_.getToken();
        } while (token.type == ignoring_);

        return token;
    }

   private:
    ILexer &lexer_;
    Token::Type ignoring_;
};

#endif
