#ifndef LEXER_H
#define LEXER_H

#include <istream>
#include <variant>

#include "errors.hpp"

struct Position {
    unsigned int line = 1;
};

struct Token {
    enum class Type {
        INT_CONST,
        BOOL_CONST,
        WHILE_KW,
    };

    Type type;
    std::variant<std::monostate, int, float, bool, std::string> value = {};
    Position position;
};

class Lexer {
   public:
    Lexer(std::istream& stream);
    Token getToken();

   private:
    std::istream& stream;
    char currentChar;
    Position currentPosition;

    char nextChar();
};

#endif
