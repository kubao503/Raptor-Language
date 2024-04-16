#include "token.hpp"

#include "magic_enum/magic_enum.hpp"

struct ToStringFunctor {
    std::string operator()(std::monostate) const { return ""; }
    std::string operator()(Integral i) const { return std::to_string(i); }
    std::string operator()(Floating i) const { return std::to_string(i); }
    std::string operator()(bool b) const { return std::to_string(b); }
    std::string operator()(const std::string& s) const { return s; }
};

std::ostream& operator<<(std::ostream& stream, const Token& token) {
    auto tokenType = magic_enum::enum_name(token.getType());
    auto tokenValue = std::visit(ToStringFunctor(), token.getValue());
    stream << '<' << tokenType << ", " << tokenValue << "> ";
    return stream;
}
