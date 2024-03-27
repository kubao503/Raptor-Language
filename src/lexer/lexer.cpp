#include "lexer.hpp"

#include <cmath>
#include <limits>
#include <string_view>

#include "errors.hpp"
#include "magic_enum/magic_enum.hpp"

Lexer::builders_map_t Lexer::initBuilders() const {
    using namespace std::placeholders;
    using std::bind;

    auto buildTwoLetterOp = bind(&Lexer::buildTwoLetterOp, this, _1, _2, _3);
    auto buildOneLetterOp = bind(&Lexer::buildOneLetterOp, this, std::placeholders::_1);
    auto buildNumber = bind(&Lexer::buildNumber, this);
    auto buildNotEqualOperator = bind(&Lexer::buildNotEqualOperator, this);
    auto buildStrConst = bind(&Lexer::buildStrConst, this);
    auto buildComment = bind(&Lexer::buildComment, this);

    return {
        {'<', bind(buildTwoLetterOp, '=', Token::Type::LT_OP, Token::Type::LTE_OP)},
        {'>', bind(buildTwoLetterOp, '=', Token::Type::GT_OP, Token::Type::GTE_OP)},
        {'=', bind(buildTwoLetterOp, '=', Token::Type::ASGN_OP, Token::Type::EQ_OP)},

        {';', bind(buildOneLetterOp, Token::Type::SEMI)},
        {',', bind(buildOneLetterOp, Token::Type::CMA)},
        {'.', bind(buildOneLetterOp, Token::Type::DOT)},
        {'+', bind(buildOneLetterOp, Token::Type::ADD_OP)},
        {'-', bind(buildOneLetterOp, Token::Type::MIN_OP)},
        {'*', bind(buildOneLetterOp, Token::Type::MULT_OP)},
        {'/', bind(buildOneLetterOp, Token::Type::DIV_OP)},
        {'(', bind(buildOneLetterOp, Token::Type::L_PAR)},
        {')', bind(buildOneLetterOp, Token::Type::R_PAR)},
        {'{', bind(buildOneLetterOp, Token::Type::L_C_BR)},
        {'}', bind(buildOneLetterOp, Token::Type::R_C_BR)},
        {EOF, bind(buildOneLetterOp, Token::Type::ETX)},

        {'0', buildNumber},
        {'1', buildNumber},
        {'2', buildNumber},
        {'3', buildNumber},
        {'4', buildNumber},
        {'5', buildNumber},
        {'6', buildNumber},
        {'7', buildNumber},
        {'8', buildNumber},
        {'9', buildNumber},

        {'!', buildNotEqualOperator},
        {'"', buildStrConst},
        {'#', buildComment},
    };
}

Token Lexer::getToken() {
    ignoreWhiteSpace();

    tokenPosition_ = source_.getPosition();

    if (auto token = buildIdOrKeyword())
        return token.value();

    auto res = builders_.find(source_.getChar());
    if (res != builders_.end()) {
        auto builder = res->second;
        return builder();
    }

    throw InvalidToken(tokenPosition_, source_.getChar());
}

void Lexer::ignoreWhiteSpace() const {
    while (std::isspace(source_.getChar())) {
        source_.nextChar();
    }
}

std::optional<Token> Lexer::buildIdOrKeyword() const {
    if (!std::isalpha(source_.getChar()))
        return std::nullopt;

    std::string lexeme;

    do {
        lexeme.push_back(source_.getChar());
        source_.nextChar();
    } while (std::isalnum(source_.getChar()) || source_.getChar() == '_');

    if (auto token = buildKeyword(lexeme))
        return token;

    if (auto token = buildBoolConst(lexeme))
        return token;

    return {{Token::Type::ID, lexeme, tokenPosition_}};
}

std::optional<Token> Lexer::buildKeyword(std::string_view lexeme) const {
    // Keyword must be lowercase
    if (std::any_of(lexeme.begin(), lexeme.end(), [](char c) { return std::isupper(c); }))
        return std::nullopt;

    auto suffix = "_KW";
    std::string keyword(lexeme.size(), ' ');
    std::transform(lexeme.begin(), lexeme.end(), keyword.begin(), ::toupper);
    keyword += suffix;

    auto res = magic_enum::enum_cast<Token::Type>(keyword);
    if (res.has_value())
        return {{res.value(), {}, tokenPosition_}};
    return std::nullopt;
}

std::optional<Token> Lexer::buildBoolConst(std::string_view lexeme) const {
    if (lexeme == "true")
        return {{Token::Type::BOOL_CONST, true, tokenPosition_}};
    else if (lexeme == "false")
        return {{Token::Type::BOOL_CONST, false, tokenPosition_}};
    return std::nullopt;
}

Token Lexer::buildNumber() const {
    integral_t value = 0;

    if (charToDigit(source_.getChar()) == 0) {
        source_.nextChar();

        if (source_.getChar() == '.')
            return buildFloat(value);
        return {Token::Type::INT_CONST, value, tokenPosition_};
    }

    do {
        auto digit = charToDigit(source_.getChar());
        if (willOverflow(value, digit))
            throw NumericOverflow(tokenPosition_, value, digit);
        value = 10 * value + digit;
        source_.nextChar();
    } while (std::isdigit(source_.getChar()));

    if (source_.getChar() == '.')
        return buildFloat(value);

    return {Token::Type::INT_CONST, value, tokenPosition_};
}

Token Lexer::buildStrConst() const {
    std::string value;
    bool escape = false;

    while (true) {
        source_.nextChar();

        if (source_.getChar() == EOF)
            throw NotTerminatedStrConst(tokenPosition_);
        else if (!escape && source_.getChar() == '"')
            break;
        else if (!escape && source_.getChar() == '\\')
            escape = true;
        else if (escape) {
            auto result = escapedChars_.find(source_.getChar());
            if (result == escapedChars_.end())
                throw NonEscapableChar(tokenPosition_, source_.getChar());

            auto escapedChar = result->second;
            value.push_back(escapedChar);
            escape = false;
        } else
            value.push_back(source_.getChar());
    }

    source_.nextChar();
    return {Token::Type::STR_CONST, value, tokenPosition_};
}

Token Lexer::buildComment() const {
    std::string value;

    while (true) {
        source_.nextChar();
        if (source_.getChar() == '\n' || source_.getChar() == EOF)
            return {Token::Type::CMT, value, tokenPosition_};
        value.push_back(source_.getChar());
    }
}

Token Lexer::buildTwoLetterOp(char second, Token::Type single, Token::Type dual) const {
    source_.nextChar();

    if (source_.getChar() == second) {
        source_.nextChar();
        return {dual, {}, tokenPosition_};
    }

    return {single, {}, tokenPosition_};
}

Token Lexer::buildOneLetterOp(Token::Type type) const {
    source_.nextChar();
    return {type, {}, tokenPosition_};
}

Token Lexer::buildNotEqualOperator() const {
    source_.nextChar();

    if (source_.getChar() == '=') {
        source_.nextChar();
        return {Token::Type::NEQ_OP, {}, tokenPosition_};
    }

    throw InvalidToken(tokenPosition_, '!');
}

Token Lexer::buildFloat(integral_t integralPart) const {
    source_.nextChar();
    if (!std::isdigit(source_.getChar()))
        throw InvalidFloat(tokenPosition_);

    integral_t fractionalPart = 0;
    int exponent = 0;
    do {
        auto digit = charToDigit(source_.getChar());
        if (willOverflow(fractionalPart, digit))
            throw NumericOverflow(tokenPosition_, fractionalPart, digit);
        fractionalPart = 10 * fractionalPart + digit;
        source_.nextChar();
        --exponent;
    } while (std::isdigit(source_.getChar()));

    floating_t value = integralPart + fractionalPart * std::pow(10, exponent);
    return {Token::Type::FLOAT_CONST, value, tokenPosition_};
}

bool Lexer::willOverflow(integral_t value, integral_t digit) {
    auto maxSafe = (std::numeric_limits<integral_t>::max() - digit) / 10;
    return value > maxSafe;
}

const std::unordered_map<char, char> Lexer::escapedChars_{
    {'n', '\n'}, {'t', '\t'}, {'"', '"'}, {'\\', '\\'}};
