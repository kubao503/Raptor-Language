#ifndef LEXER_H
#define LEXER_H

#include <cmath>
#include <functional>
#include <limits>
#include <string_view>
#include <unordered_map>

#include "errors.hpp"
#include "source.hpp"
#include "token.hpp"

class Lexer {
    using builders_map_t = std::unordered_map<char, std::function<Token()>>;

   public:
    Lexer(Source& source)
        : source_(source), builders_(initBuilders()) {}

    Token getToken();

   private:
    Source& source_;
    Position tokenPosition_;
    const builders_map_t builders_;

    builders_map_t initBuilders() const;
    void ignoreWhiteSpace() const;
    std::optional<Token> buildIdOrKeyword() const;
    std::optional<Token> buildKeyword(std::string_view lexeme) const;
    std::optional<Token> buildBoolConst(std::string_view lexeme) const;
    Token buildTwoLetterOp(char second, Token::Type single, Token::Type dual) const;
    Token buildOneLetterOp(Token::Type type) const;
    Token buildNotEqualOperator() const;
    Token buildFloat(integral_t integralPart) const;
    Token buildNumber() const;
    Token buildStrConst() const;
    Token buildComment() const;

    static const std::unordered_map<std::string_view, Token::Type> keywords_;

    static integral_t charToDigit(char c) { return c - '0'; }
    static bool willOverflow(integral_t value, integral_t digit);
};

#endif
