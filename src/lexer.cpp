#include "lexer.hpp"

Lexer::Lexer(std::istream& stream)
    : stream(stream) {}

Token Lexer::getToken() {
    char c;
    stream.get(c);
    return {Token::Type::BOOL_CONST, true};
}
