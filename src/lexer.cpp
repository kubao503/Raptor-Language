#include "lexer.hpp"

Lexer::Lexer(std::istream& stream)
    : stream(stream) {
    stream.get(currentChar);
}

Token Lexer::getToken() {
    while (std::isspace(currentChar)) {
        if (currentChar == '\n')
            currentPosition.line += 1;

        currentChar = nextChar();
    }

    if (std::isalpha(currentChar)) {
        std::string lexeme;

        lexeme.push_back(currentChar);
        currentChar = nextChar();

        while (std::isalpha(currentChar)) {
            lexeme.push_back(currentChar);
            currentChar = nextChar();
        }

        if (lexeme == "true")
            return {Token::Type::BOOL_CONST, true, currentPosition};
        else if (lexeme == "while")
            return {Token::Type::WHILE_KW, {}, currentPosition};
    }
    throw InvalidToken(currentChar);
}

char Lexer::nextChar() {
    char c;
    stream.get(c);
    return c;
}
