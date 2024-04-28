#ifndef PARSER_H
#define PARSER_H

#include "../lexer/token.hpp"
#include "program.hpp"

class ILexer;

/// @brief Parser building parse tree from tokens
class Parser {
   public:
    explicit Parser(ILexer& lexer)
        : lexer_(lexer) {}

    /// @brief Builds parse tree from token acquired from lexer
    /// @return Parse tree
    Program parseProgram();

   private:
    ILexer& lexer_;
    Token currentToken_;
};

#endif
