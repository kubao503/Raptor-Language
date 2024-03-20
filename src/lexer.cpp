#include "lexer.hpp"

Token Lexer::getToken() {
    ignoreWhiteSpace();

    tokenPosition_ = source_.getPosition();

    if (std::isalpha(source_.getChar()))
        return handleIdAndKeyword();

    if (std::isdigit(source_.getChar()))
        return handleNum();

    throw InvalidToken(source_.getChar());
}

const std::unordered_map<std::string, Token::Type> Lexer::keywords = {
    {"if", Token::Type::IF_KW},
    {"while", Token::Type::WHILE_KW},
    {"return", Token::Type::RETURN_KW},
    {"print", Token::Type::PRINT_KW},
    {"const", Token::Type::CONST_KW},
    {"ref", Token::Type::REF_KW},
    {"struct", Token::Type::STRUCT_KW},
    {"variant", Token::Type::VARIANT_KW},
    {"or", Token::Type::OR_KW},
    {"and", Token::Type::AND_KW},
    {"not", Token::Type::NOT_KW},
    {"as", Token::Type::AS_KW},
    {"is", Token::Type::IS_KW},
    {"void", Token::Type::VOID_KW},
    {"int", Token::Type::INT_KW},
    {"float", Token::Type::FLOAT_KW},
    {"bool", Token::Type::BOOL_KW},
    {"str", Token::Type::STR_KW},
};

void Lexer::ignoreWhiteSpace() {
    while (std::isspace(source_.getChar())) {
        source_.nextChar();
    }
}

Token Lexer::handleIdAndKeyword() {
    std::string lexeme;

    do {
        lexeme.push_back(source_.getChar());
        source_.nextChar();
    } while (std::isalnum(source_.getChar()) || source_.getChar() == '_');

    auto k = keywords.find(lexeme);
    if (k != keywords.end())
        return {k->second, {}, tokenPosition_};

    if (lexeme == "true")
        return {Token::Type::BOOL_CONST, true, tokenPosition_};

    return {Token::Type::ID, lexeme, tokenPosition_};
}

Token Lexer::handleNum() {
    unsigned int num = 0;

    if (charToDigit(source_.getChar()) == 0) {
        source_.nextChar();

        if (source_.getChar() == '.')
            return handleFloat(num);
        else if (std::isdigit(source_.getChar()))
            throw InvalidToken(source_.getChar());

        return {Token::Type::INT_CONST, num, tokenPosition_};
    }

    do {
        num = 10 * num + charToDigit(source_.getChar());
        source_.nextChar();
    } while (std::isdigit(source_.getChar()));

    if (source_.getChar() == '.')
        return handleFloat(num);

    return {Token::Type::INT_CONST, num, tokenPosition_};
}

Token Lexer::handleFloat(unsigned int integralPart) {
    source_.nextChar();
    if (!std::isdigit(source_.getChar()))
        throw InvalidToken(source_.getChar());

    unsigned int fractionalPart = 0;
    int exponent = 0;
    do {
        fractionalPart = 10 * fractionalPart + charToDigit(source_.getChar());
        source_.nextChar();
        --exponent;
    } while (std::isdigit(source_.getChar()));

    float value = integralPart + fractionalPart * std::pow(10, exponent);
    return {Token::Type::FLOAT_CONST, value, tokenPosition_};
}
