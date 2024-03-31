#include "lexer.hpp"

#include <cmath>
#include <limits>
#include <string_view>

#include "errors.hpp"
#include "magic_enum/magic_enum.hpp"

Token Lexer::getToken() {
    ignoreWhiteSpace();

    tokenPosition_ = source_->getPosition();

    for (const auto& builder : TokenBuilders_) {
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
    static std::string suffix{"_KW"};

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

std::optional<Token> Lexer::buildIntConst() const {
    if (source_->getChar() == '0') {
        source_->nextChar();

        if (auto token = buildFloatConst(0u))
            return token;

        return {{Token::Type::INT_CONST, 0u, tokenPosition_}};
    }

    if (auto res = buildNumber()) {
        auto [integralPart, _] = res.value();

        if (auto token = buildFloatConst(integralPart))
            return token;

        return {{Token::Type::INT_CONST, integralPart, tokenPosition_}};
    }

    return std::nullopt;
}

std::optional<Token> Lexer::buildFloatConst(Integral integralPart) const {
    if (source_->getChar() != '.')
        return std::nullopt;

    source_->nextChar();

    if (auto res = buildNumber()) {
        auto [fractionalPart, digitCount] = res.value();
        int exponent{-static_cast<int>(digitCount)};

        Floating value = integralPart + fractionalPart * std::pow(10, exponent);
        return {{Token::Type::FLOAT_CONST, value, tokenPosition_}};
    }

    throw InvalidFloat(tokenPosition_);
}

std::optional<Lexer::IntWithDigitCount> Lexer::buildNumber() const {
    if (!std::isdigit(source_->getChar()))
        return std::nullopt;

    Integral value{0};
    unsigned int digitCount{0};

    do {
        auto digit = charToDigit(source_->getChar());
        if (willOverflow(value, digit))
            throw NumericOverflow(tokenPosition_, value, digit);
        value = 10 * value + digit;
        source_->nextChar();
        ++digitCount;
    } while (std::isdigit(source_->getChar()));

    return {{value, digitCount}};
}

std::optional<Token> Lexer::buildStrConst() const {
    if (source_->getChar() != '"')
        return std::nullopt;
    source_->nextChar();

    std::string strConst;

    while (source_->getChar() != '"') {
        expectNoEndOfFile();
        auto strConstChar = source_->getChar();

        if (source_->getChar() == '\\') {
            source_->nextChar();
            expectNoEndOfFile();
            strConstChar = findInEscapedChars(source_->getChar());
        }

        strConst.push_back(strConstChar);
        source_->nextChar();
    }

    source_->nextChar();
    return {{Token::Type::STR_CONST, strConst, tokenPosition_}};
}

void Lexer::expectNoEndOfFile() const {
    if (source_->getChar() == EOF)
        throw NotTerminatedStrConst(tokenPosition_);
}

char Lexer::findInEscapedChars(char searched) const {
    auto pred = [searched](const CharPair& p) { return p.first == searched; };
    auto res = std::find_if(escapedChars_.begin(), escapedChars_.end(), pred);

    if (res == escapedChars_.end())
        throw NonEscapableChar(tokenPosition_, source_->getChar());
    return res->second;
}

std::optional<Token> Lexer::buildComment() const {
    if (source_->getChar() != '#')
        return std::nullopt;
    source_->nextChar();

    std::string value;

    while (source_->getChar() != '\n' && source_->getChar() != EOF) {
        value.push_back(source_->getChar());
        source_->nextChar();
    }
    return {{Token::Type::CMT, value, tokenPosition_}};
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

std::optional<Token> Lexer::buildTwoLetterOp(CharPair chars, TokenTypes types) const {
    if (source_->getChar() != chars.first)
        return std::nullopt;

    source_->nextChar();

    if (source_->getChar() != chars.second)
        return {{types.first, {}, tokenPosition_}};

    source_->nextChar();
    return {{types.second, {}, tokenPosition_}};
}

bool Lexer::willOverflow(Integral value, Integral digit) {
    // 10 * maxSafe + digit <= max
    auto maxSafe = (std::numeric_limits<Integral>::max() - digit) / 10;
    return value > maxSafe;
}

Lexer::TokenBuilders Lexer::TokenBuilders_{
    [](Lexer& l) { return l.buildIdOrKeyword(); },
    [](Lexer& l) { return l.buildIntConst(); },
    [](Lexer& l) { return l.buildStrConst(); },
    [](Lexer& l) { return l.buildComment(); },
    [](Lexer& l) { return l.buildNotEqualOp(); },

    [](Lexer& l) { return l.buildOneLetterOp(';', Token::Type::SEMI); },
    [](Lexer& l) { return l.buildOneLetterOp(',', Token::Type::CMA); },
    [](Lexer& l) { return l.buildOneLetterOp('.', Token::Type::DOT); },
    [](Lexer& l) { return l.buildOneLetterOp('+', Token::Type::ADD_OP); },
    [](Lexer& l) { return l.buildOneLetterOp('-', Token::Type::MIN_OP); },
    [](Lexer& l) { return l.buildOneLetterOp('*', Token::Type::MULT_OP); },
    [](Lexer& l) { return l.buildOneLetterOp('/', Token::Type::DIV_OP); },
    [](Lexer& l) { return l.buildOneLetterOp('(', Token::Type::L_PAR); },
    [](Lexer& l) { return l.buildOneLetterOp(')', Token::Type::R_PAR); },
    [](Lexer& l) { return l.buildOneLetterOp('{', Token::Type::L_C_BR); },
    [](Lexer& l) { return l.buildOneLetterOp('}', Token::Type::R_C_BR); },
    [](Lexer& l) { return l.buildOneLetterOp(EOF, Token::Type::ETX); },

    [](Lexer& l) {
        return l.buildTwoLetterOp({'<', '='}, {Token::Type::LT_OP, Token::Type::LTE_OP});
    },
    [](Lexer& l) {
        return l.buildTwoLetterOp({'>', '='}, {Token::Type::GT_OP, Token::Type::GTE_OP});
    },
    [](Lexer& l) {
        return l.buildTwoLetterOp({'=', '='}, {Token::Type::ASGN_OP, Token::Type::EQ_OP});
    },
};

Lexer::EscapedChars Lexer::escapedChars_{
    {'n', '\n'}, {'t', '\t'}, {'"', '"'}, {'\\', '\\'}};
