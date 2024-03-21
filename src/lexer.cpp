#include "lexer.hpp"

Token Lexer::getToken() {
    ignoreWhiteSpace();

    tokenPosition_ = source_.getPosition();

    if (std::isalpha(source_.getChar()))
        return buildIdOrKeyword();

    auto res = builders_.find(source_.getChar());
    if (res != builders_.end()) {
        auto builder = res->second;
        return builder();
    }

    throw InvalidToken(source_.getChar());
}

void Lexer::ignoreWhiteSpace() {
    while (std::isspace(source_.getChar())) {
        source_.nextChar();
    }
}

Token Lexer::buildIdOrKeyword() {
    std::string lexeme;

    do {
        lexeme.push_back(source_.getChar());
        source_.nextChar();
    } while (std::isalnum(source_.getChar()) || source_.getChar() == '_');

    auto k = keywords_.find(lexeme);
    if (k != keywords_.end())
        return {k->second, {}, tokenPosition_};

    if (lexeme == "true")
        return {Token::Type::BOOL_CONST, true, tokenPosition_};

    return {Token::Type::ID, lexeme, tokenPosition_};
}

Token Lexer::buildNumber() {
    integral_t value = 0;

    if (charToDigit(source_.getChar()) == 0) {
        source_.nextChar();

        if (source_.getChar() == '.')
            return buildFloat(value);
        else if (std::isdigit(source_.getChar()))
            throw InvalidToken(source_.getChar());

        return {Token::Type::INT_CONST, value, tokenPosition_};
    }

    do {
        auto digit = charToDigit(source_.getChar());
        if (willOverflow(value, digit))
            throw IntOverflow();
        value = 10 * value + digit;
        source_.nextChar();
    } while (std::isdigit(source_.getChar()));

    if (source_.getChar() == '.')
        return buildFloat(value);

    return {Token::Type::INT_CONST, value, tokenPosition_};
}

Token Lexer::buildFloat(integral_t integralPart) {
    source_.nextChar();
    if (!std::isdigit(source_.getChar()))
        throw InvalidToken(source_.getChar());

    integral_t fractionalPart = 0;
    int exponent = 0;
    do {
        auto digit = charToDigit(source_.getChar());
        if (willOverflow(fractionalPart, digit))
            throw FloatOverflow();
        fractionalPart = 10 * fractionalPart + digit;
        source_.nextChar();
        --exponent;
    } while (std::isdigit(source_.getChar()));

    floating_t value = integralPart + fractionalPart * std::pow(10, exponent);
    return {Token::Type::FLOAT_CONST, value, tokenPosition_};
}

const std::unordered_map<std::string, Token::Type> Lexer::keywords_{
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

bool Lexer::willOverflow(integral_t value, integral_t digit) {
    auto maxSafe = (std::numeric_limits<integral_t>::max() - digit) / 10;
    return value > maxSafe;
}
