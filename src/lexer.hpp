#ifndef LEXER_H
#define LEXER_H

#include <cmath>
#include <functional>
#include <limits>
#include <unordered_map>
#include <variant>

#include "errors.hpp"
#include "source.hpp"

using integral_t = unsigned int;
using floating_t = float;

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
    std::variant<std::monostate, integral_t, floating_t, bool, std::string> value = {};
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

    const std::unordered_map<char, std::function<Token()>> builders_ = {
        {'0', std::bind(&Lexer::buildNumber, this)},
        {'1', std::bind(&Lexer::buildNumber, this)},
        {'2', std::bind(&Lexer::buildNumber, this)},
        {'3', std::bind(&Lexer::buildNumber, this)},
        {'4', std::bind(&Lexer::buildNumber, this)},
        {'5', std::bind(&Lexer::buildNumber, this)},
        {'6', std::bind(&Lexer::buildNumber, this)},
        {'7', std::bind(&Lexer::buildNumber, this)},
        {'8', std::bind(&Lexer::buildNumber, this)},
        {'9', std::bind(&Lexer::buildNumber, this)},
        {EOF, [this]() -> Token { return {Token::Type::ETX, {}, tokenPosition_}; }},
    };

    void
    ignoreWhiteSpace();
    Token buildIdOrKeyword();
    Token buildFloat(integral_t integralPart);
    Token buildNumber();

    static const std::unordered_map<std::string, Token::Type> keywords_;

    static integral_t charToDigit(char c) { return c - '0'; }
    static bool willOverflow(integral_t value, integral_t digit);
};

#endif
