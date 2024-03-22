#include "token.hpp"

const std::unordered_map<Token::Type, std::string> Token::typeNames = {
    {Type::IF_KW, "IF_KW"},
    {Type::WHILE_KW, "WHILE_KW"},
    {Type::RETURN_KW, "RETURN_KW"},
    {Type::PRINT_KW, "PRINT_KW"},
    {Type::CONST_KW, "CONST_KW"},
    {Type::REF_KW, "REF_KW"},
    {Type::STRUCT_KW, "STRUCT_KW"},
    {Type::VARIANT_KW, "VARIANT_KW"},
    {Type::OR_KW, "OR_KW"},
    {Type::AND_KW, "AND_KW"},
    {Type::NOT_KW, "NOT_KW"},
    {Type::AS_KW, "AS_KW"},
    {Type::IS_KW, "IS_KW"},
    {Type::VOID_KW, "VOID_KW"},
    {Type::INT_KW, "INT_KW"},
    {Type::FLOAT_KW, "FLOAT_KW"},
    {Type::BOOL_KW, "BOOL_KW"},
    {Type::STR_KW, "STR_KW"},
    {Type::ID, "ID"},
    {Type::BOOL_CONST, "BOOL_CONST"},
    {Type::INT_CONST, "INT_CONST"},
    {Type::FLOAT_CONST, "FLOAT_CONST"},
    {Type::STR_CONST, "STR_CONST"},
    {Type::ETX, "ETX"},
    {Type::LT_OP, "LT_OP"},
    {Type::LTE_OP, "LTE_OP"},
    {Type::GT_OP, "GT_OP"},
    {Type::GTE_OP, "GTE_OP"},
    {Type::ASGN_OP, "ASGN_OP"},
    {Type::EQ_OP, "EQ_OP"},
    {Type::NEQ_OP, "NEQ_OP"},
    {Type::ADD_OP, "ADD_OP"},
    {Type::MIN_OP, "MIN_OP"},
    {Type::MULT_OP, "MULT_OP"},
    {Type::DIV_OP, "DIV_OP"},
    {Type::SEMI, "SEMI"},
    {Type::CMA, "CMA"},
    {Type::DOT, "DOT"},
    {Type::L_PAR, "L_PAR"},
    {Type::R_PAR, "R_PAR"},
    {Type::L_C_BR, "L_C_BR"},
    {Type::R_C_BR, "R_C_BR"},
    {Type::CMT, "CMT"},
};

struct ToStringFunctor {
    std::string operator()(std::monostate) const { return ""; }
    std::string operator()(integral_t i) const { return std::to_string(i); }
    std::string operator()(floating_t i) const { return std::to_string(i); }
    std::string operator()(bool b) const { return std::to_string(b); }
    std::string operator()(const std::string& s) const { return s; }
};

std::ostream& operator<<(std::ostream& stream, const Token& token) {
    stream << '<' << Token::typeNames.at(token.type) << ", "
           << std::visit(ToStringFunctor(), token.value) << "> ";
    return stream;
}
