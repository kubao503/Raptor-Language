#include "lexer.hpp"

#include <cmath>
#include <limits>
#include <string_view>

#include "errors.hpp"
#include "magic_enum/magic_enum.hpp"

Token Lexer::getToken() {
    ignoreWhiteSpace();

    tokenPosition_ = source_->getPosition();

    for (const auto& builder : builders_) {
        if (auto token = builder(*this))
            return token.value();
    }

    throw InvalidToken(tokenPosition_, source_->getChar());
}

void Lexer::ignoreWhiteSpace() const {
    while (std::isspace(source_->getChar())) {
        source_->nextChar();
    }
}

bool isAlnumOrUnderscore(char c) {
    return std::isalnum(c) || c == '_';
}

std::optional<Token> Lexer::buildIdOrKeyword() const {
    if (!std::isalpha(source_->getChar()))
        return std::nullopt;

    std::string lexeme;

    do {
        lexeme.push_back(source_->getChar());
        source_->nextChar();
    } while (isAlnumOrUnderscore(source_->getChar()));

    if (auto token = buildKeyword(lexeme))
        return token;

    if (auto token = buildBoolConst(lexeme))
        return token;

    return {{Token::Type::ID, lexeme, tokenPosition_}};
}

bool isAnyUpperCase(std::string_view s) {
    return std::any_of(s.begin(), s.end(), [](char c) { return std::isupper(c); });
}

std::optional<Token> Lexer::buildKeyword(std::string_view lexeme) const {
    if (isAnyUpperCase(lexeme))
        return std::nullopt;

    auto keyword = lexemeToKeyword(lexeme);

    if (auto tokenType = magic_enum::enum_cast<Token::Type>(keyword))
        return {{tokenType.value(), {}, tokenPosition_}};
    return std::nullopt;
}

std::string Lexer::lexemeToKeyword(std::string_view lexeme) {
    std::string suffix = "_KW";
    std::string keyword(lexeme.size(), ' ');
    std::transform(lexeme.begin(), lexeme.end(), keyword.begin(), ::toupper);
    keyword += suffix;

    return keyword;
}

std::optional<Token> Lexer::buildBoolConst(std::string_view lexeme) const {
    if (lexeme == "true")
        return {{Token::Type::TRUE_CONST, {}, tokenPosition_}};
    else if (lexeme == "false")
        return {{Token::Type::FALSE_CONST, {}, tokenPosition_}};
    return std::nullopt;
}

std::optional<Token> Lexer::buildNumber() const {
    if (!std::isdigit(source_->getChar()))
        return std::nullopt;

    integral_t value = 0;

    if (charToDigit(source_->getChar()) == 0) {
        source_->nextChar();

        if (auto token = buildFloat(value))
            return token;

        return {{Token::Type::INT_CONST, value, tokenPosition_}};
    }

    do {
        auto digit = charToDigit(source_->getChar());
        if (willOverflow(value, digit))
            throw NumericOverflow(tokenPosition_, value, digit);
        value = 10 * value + digit;
        source_->nextChar();
    } while (std::isdigit(source_->getChar()));

    if (auto token = buildFloat(value))
        return token;

    return {{Token::Type::INT_CONST, value, tokenPosition_}};
}

std::optional<Token> Lexer::buildFloat(integral_t integralPart) const {
    if (source_->getChar() != '.')
        return std::nullopt;

    source_->nextChar();
    if (!std::isdigit(source_->getChar()))
        throw InvalidFloat(tokenPosition_);

    integral_t fractionalPart = 0;
    int exponent = 0;
    do {
        auto digit = charToDigit(source_->getChar());
        if (willOverflow(fractionalPart, digit))
            throw NumericOverflow(tokenPosition_, fractionalPart, digit);
        fractionalPart = 10 * fractionalPart + digit;
        source_->nextChar();
        --exponent;
    } while (std::isdigit(source_->getChar()));

    floating_t value = integralPart + fractionalPart * std::pow(10, exponent);
    return {{Token::Type::FLOAT_CONST, value, tokenPosition_}};
}

std::optional<Token> Lexer::buildStrConst() const {
    if (source_->getChar() != '"')
        return std::nullopt;

    std::string value;
    bool escape = false;

    while (true) {
        source_->nextChar();

        if (source_->getChar() == EOF)
            throw NotTerminatedStrConst(tokenPosition_);
        else if (!escape && source_->getChar() == '"')
            break;
        else if (!escape && source_->getChar() == '\\')
            escape = true;
        else if (escape) {
            auto result = escapedChars_.find(source_->getChar());
            if (result == escapedChars_.end())
                throw NonEscapableChar(tokenPosition_, source_->getChar());

            auto escapedChar = result->second;
            value.push_back(escapedChar);
            escape = false;
        } else
            value.push_back(source_->getChar());
    }

    source_->nextChar();
    return {{Token::Type::STR_CONST, value, tokenPosition_}};
}

std::optional<Token> Lexer::buildComment() const {
    if (source_->getChar() != '#')
        return std::nullopt;

    std::string value;

    while (true) {
        source_->nextChar();
        if (source_->getChar() == '\n' || source_->getChar() == EOF)
            return {{Token::Type::CMT, value, tokenPosition_}};
        value.push_back(source_->getChar());
    }
}

std::optional<Token> Lexer::buildNotEqualOp() const {
    if (source_->getChar() != '!')
        return std::nullopt;

    source_->nextChar();

    if (source_->getChar() == '=') {
        source_->nextChar();
        return {{Token::Type::NEQ_OP, {}, tokenPosition_}};
    }

    throw InvalidToken(tokenPosition_, '!');
}

std::optional<Token> Lexer::buildOneLetterOp(char c, Token::Type type) const {
    if (source_->getChar() != c)
        return std::nullopt;

    source_->nextChar();
    return {{type, {}, tokenPosition_}};
}

std::optional<Token> Lexer::buildTwoLetterOp(chars_t chars, token_types_t types) const {
    if (source_->getChar() != chars.first)
        return std::nullopt;

    source_->nextChar();

    if (source_->getChar() != chars.second)
        return {{types.first, {}, tokenPosition_}};

    source_->nextChar();
    return {{types.second, {}, tokenPosition_}};
}

bool Lexer::willOverflow(integral_t value, integral_t digit) {
    // 10 * maxSafe + digit <= max
    auto maxSafe = (std::numeric_limits<integral_t>::max() - digit) / 10;
    return value > maxSafe;
}

const Lexer::builders_t Lexer::builders_{
    [](Lexer& lexer) { return lexer.buildIdOrKeyword(); },
    [](Lexer& lexer) { return lexer.buildNumber(); },
    [](Lexer& lexer) { return lexer.buildStrConst(); },
    [](Lexer& lexer) { return lexer.buildComment(); },
    [](Lexer& lexer) { return lexer.buildNotEqualOp(); },

    [](Lexer& lexer) { return lexer.buildOneLetterOp(';', Token::Type::SEMI); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp(',', Token::Type::CMA); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp('.', Token::Type::DOT); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp('+', Token::Type::ADD_OP); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp('-', Token::Type::MIN_OP); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp('*', Token::Type::MULT_OP); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp('/', Token::Type::DIV_OP); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp('(', Token::Type::L_PAR); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp(')', Token::Type::R_PAR); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp('{', Token::Type::L_C_BR); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp('}', Token::Type::R_C_BR); },
    [](Lexer& lexer) { return lexer.buildOneLetterOp(EOF, Token::Type::ETX); },

    [](Lexer& lexer) {
        return lexer.buildTwoLetterOp({'<', '='},
                                      {Token::Type::LT_OP, Token::Type::LTE_OP});
    },
    [](Lexer& lexer) {
        return lexer.buildTwoLetterOp({'>', '='},
                                      {Token::Type::GT_OP, Token::Type::GTE_OP});
    },
    [](Lexer& lexer) {
        return lexer.buildTwoLetterOp({'=', '='},
                                      {Token::Type::ASGN_OP, Token::Type::EQ_OP});
    },
};

const std::unordered_map<char, char> Lexer::escapedChars_{
    {'n', '\n'}, {'t', '\t'}, {'"', '"'}, {'\\', '\\'}};
