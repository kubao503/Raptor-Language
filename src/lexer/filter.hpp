#ifndef FILTER_H
#define FILTER_H

#include "ILexer.hpp"

class InvalidFilterType : public std::exception {
   public:
    const char* what() const noexcept override {
        return "Cannot specify ETX as ignore type";
    }
};

class Filter : public ILexer {
   public:
    Filter(ILexer* lexer, Token::Type ignore)
        : lexer_(lexer), ignore_(ignore) {
        if (ignore == Token::Type::ETX)
            throw InvalidFilterType();
    }

    Token getToken() override {
        Token token;

        do {
            token = lexer_->getToken();
        } while (token.getType() == ignore_);

        return token;
    }

   private:
    ILexer* lexer_;
    Token::Type ignore_;
};

#endif
