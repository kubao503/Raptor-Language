#ifndef LEXER_H
#define LEXER_H

#include <cmath>
#include <istream>
#include <unordered_map>
#include <variant>

#include "errors.hpp"

struct Position {
    unsigned int line = 1;
};

struct Token {
    enum class Type {
        IF_KW,
        WHILE_KW,
        RETURN_KW,
        PRINT_KW,
        CONST_KW,
        REF_KW,
        STRUCT_KW,
        VARIANT_KW,
        OR_KW,
        AND_KW,
        NOT_KW,
        AS_KW,
        IS_KW,
        VOID_KW,
        INT_KW,
        FLOAT_KW,
        BOOL_KW,
        STR_KW,
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

    static const std::unordered_map<std::string, Token::Type> keywords;
};

#endif
