#include "lexer.hpp"

Lexer::Lexer(std::istream& stream)
    : stream(stream) {
    stream.get(currentChar);
}

Token Lexer::getToken() {
    ignoreWhiteSpace();

    if (std::isalpha(currentChar))
        return handleIdAndKeyword();

    throw InvalidToken(currentChar);
}

void Lexer::ignoreWhiteSpace() {
    while (std::isspace(currentChar)) {
        if (currentChar == '\n')
            currentPosition.line += 1;

        currentChar = nextChar();
    }
}

Token Lexer::handleIdAndKeyword() {
    std::string lexeme;

    do {
        lexeme.push_back(currentChar);
        currentChar = nextChar();
    } while (std::isalnum(currentChar) || currentChar == '_');

    if (lexeme == "true")
        return {Token::Type::BOOL_CONST, true, currentPosition};
    else if (lexeme == "while")
        return {Token::Type::WHILE_KW, {}, currentPosition};
    return {Token::Type::ID, lexeme, currentPosition};
}

char Lexer::nextChar() {
    char c;
    stream.get(c);
    return c;
}
