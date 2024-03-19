#ifndef LEXER_H
#define LEXER_H

#include <cmath>
#include <istream>
#include <variant>

#include "errors.hpp"

struct Position {
    unsigned int line = 1;
};

struct Token {
    enum class Type {
        WHILE_KW,
        ID,
        BOOL_CONST,
        INT_CONST,
        FLOAT_CONST,
    };

    Type type;
    std::variant<std::monostate, unsigned int, float, bool, std::string> value = {};
    Position position;
};

class Lexer {
   public:
    Lexer(std::istream& stream);
    Token getToken();

   private:
    std::istream& stream;

    Token currentToken;
    char currentChar;
    Position currentPosition;

    void ignoreWhiteSpace();
    Token handleIdAndKeyword();
    Token handleFloat(unsigned int integralPart);
    Token handleNum();
    char nextChar();
};

#endif
