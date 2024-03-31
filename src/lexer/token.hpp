#ifndef TOKEN_H
#define TOKEN_H

#include <ostream>
#include <variant>

#include "position.hpp"
#include "types.hpp"

struct Position;

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
        TRUE_CONST,
        FALSE_CONST,
        INT_CONST,
        FLOAT_CONST,
        STR_CONST,
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
        CMA,
        DOT,
        L_PAR,
        R_PAR,
        L_C_BR,
        R_C_BR,
        CMT,
    };

    Type type;
    std::variant<std::monostate, Integral, Floating, bool, std::string> value = {};
    Position position;
};

std::ostream& operator<<(std::ostream& stream, const Token& token);

#endif
