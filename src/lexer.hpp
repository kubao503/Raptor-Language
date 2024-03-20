#ifndef LEXER_H
#define LEXER_H

#include <cmath>
#include <unordered_map>
#include <variant>

#include "errors.hpp"
#include "source.hpp"

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
        ETX,
    };

    Type type;
    std::variant<std::monostate, unsigned int, float, bool, std::string> value = {};
    Position position;
};

class Lexer {
   public:
    Lexer(Source& source)
        : source_(source) {}

    Token getToken();

   private:
    Source& source_;
    Position tokenPosition_;

    void ignoreWhiteSpace();
    Token handleIdAndKeyword();
    Token handleFloat(unsigned int integralPart);
    Token handleNum();

    static const std::unordered_map<std::string, Token::Type> keywords;

    static unsigned int charToDigit(char c) { return c - '0'; }
};

#endif
