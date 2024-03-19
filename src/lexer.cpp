#include "lexer.hpp"

Lexer::Lexer(std::istream& stream)
    : stream(stream) {
    stream.get(currentChar);
}

Token Lexer::getToken() {
    ignoreWhiteSpace();

    if (std::isalpha(currentChar))
        return handleIdAndKeyword();

    if (std::isdigit(currentChar))
        return handleNum();

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

Token Lexer::handleNum() {
    unsigned int num = 0;

    if (std::atoi(&currentChar) == 0) {
        currentChar = nextChar();

        if (currentChar == '.')
            return handleFloat(num);
        else if (std::isdigit(currentChar))
            throw InvalidToken(currentChar);

        return {Token::Type::INT_CONST, num, currentPosition};
    }

    do {
        num = 10 * num + std::atoi(&currentChar);
        currentChar = nextChar();
    } while (std::isdigit(currentChar));

    if (currentChar == '.')
        return handleFloat(num);

    return {Token::Type::INT_CONST, num, currentPosition};
}

Token Lexer::handleFloat(unsigned int integralPart) {
    currentChar = nextChar();
    if (!std::isdigit(currentChar))
        throw InvalidToken(currentChar);

    unsigned int fractionalPart = 0;
    int exponent = 0;
    do {
        fractionalPart = 10 * fractionalPart + std::atoi(&currentChar);
        currentChar = nextChar();
        --exponent;
    } while (std::isdigit(currentChar));

    float value = integralPart + fractionalPart * std::pow(10, exponent);
    return {Token::Type::FLOAT_CONST, value, currentPosition};
}

char Lexer::nextChar() {
    char c;
    stream.get(c);
    return c;
}
