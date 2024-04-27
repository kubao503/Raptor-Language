#ifndef FILTER_H
#define FILTER_H

#include "ILexer.hpp"

/// @brief Exception thrown when trying to instantiate
/// a Filter ignoring end-of-text tokens
class InvalidFilterType : public std::exception {
   public:
    const char* what() const noexcept override {
        return "Cannot specify ETX as ignore type";
    }
};

/// @brief Filter ignoring given token type. Implements the same interface as Lexer.
///
/// It is if fact a decorator for the ILexer
class Filter : public ILexer {
   public:
    /// @brief Constructs filter that decorates lexer and ignores tokens of ignore type
    ///
    /// Ignoring end-of-text token is not allowed as this could result in infinite loop
    /// @param lexer
    /// @param ignore
    Filter(ILexer& lexer, Token::Type ignore)
        : lexer_(lexer), ignore_(ignore) {
        if (ignore == Token::Type::ETX)
            throw InvalidFilterType();
    }

    /// @brief Returns a token from the decorated ILexer. Requests another token until the
    /// token returned to be of the ignored type
    /// @return Token from decorated ILexer of type other than the ignored one
    Token getToken() override {
        Token token;

        do {
            token = lexer_.getToken();
        } while (token.getType() == ignore_);

        return token;
    }

   private:
    ILexer& lexer_;
    Token::Type ignore_;
};

#endif
