#include "lexer.hpp"

char getToken(std::istream& stream) {
    char c;
    stream.get(c);
    return c;
}
