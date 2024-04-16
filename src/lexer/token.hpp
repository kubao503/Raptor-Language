#ifndef TOKEN_H
#define TOKEN_H

#include <ostream>
#include <variant>

#include "position.hpp"
#include "types.hpp"

struct Position;

class Token {
   public:
    enum class Type {
        UNKNOWN,
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

    using Value = std::variant<std::monostate, Integral, Floating, bool, std::string>;

    Token(Type type, Value value, Position position)
        : type_(type), value_(value), position_(position) {}
    Token()
        : type_(Type::UNKNOWN) {}

    Type getType() const { return type_; }
    const Value& getValue() const { return value_; }
    const Position& getPosition() const { return position_; }

   private:
    Type type_;
    Value value_ = {};
    Position position_;
};

std::ostream& operator<<(std::ostream& stream, const Token& token);

#endif
