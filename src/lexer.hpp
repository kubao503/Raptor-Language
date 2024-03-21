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
        LT_OP,
        LTE_OP,
        GT_OP,
        GTE_OP,
        ASGN_OP,
        EQ_OP,
        NEQ_OP,
        ADD_OP,
        MIN_OP,
        MULT_OP,
        DIV_OP,
        SEMI,
        COM,
        DOT,
        L_PAR,
        R_PAR,
        L_C_BR,
        R_C_BR,
    };

    Type type;
    std::variant<std::monostate, integral_t, floating_t, bool, std::string> value = {};
    Position position;

    operator bool() { return !std::holds_alternative<std::monostate>(value); }
};

class Lexer {
    using builders_map_t = std::unordered_map<char, std::function<Token()>>;

   public:
    Lexer(Source& source)
        : source_(source), builders_(initBuilders()) {}

    Token getToken();

   private:
    Source& source_;
    Position tokenPosition_;
    const builders_map_t builders_;

    builders_map_t initBuilders();
    void ignoreWhiteSpace();
    std::optional<Token> buildIdOrKeyword();
    Token buildTwoLetterOp(char second, Token::Type single, Token::Type dual);
    Token buildOneLetterOp(Token::Type type);
    Token buildNotEqualOperator();
    Token buildFloat(integral_t integralPart);
    Token buildNumber();

    static const std::unordered_map<std::string, Token::Type> keywords_;

    static integral_t charToDigit(char c) { return c - '0'; }
    static bool willOverflow(integral_t value, integral_t digit);
};

#endif
