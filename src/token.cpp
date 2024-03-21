#include "token.hpp"

std::ostream& operator<<(std::ostream& stream, const Token& token) {
    stream << '<' << static_cast<int>(token.type) << "> ";
    return stream;
}
