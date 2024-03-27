#ifndef LEXER_H
#define LEXER_H

#include <functional>
#include <optional>
#include <unordered_map>

#include "ILexer.hpp"
#include "source.hpp"
#include "token.hpp"

class Lexer : public ILexer {
    using builders_map_t = std::unordered_map<char, std::function<Token()>>;

   public:
    Lexer(Source& source)
        : source_(source), builders_(initBuilders()) {}

    Token getToken() override;

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

    static integral_t charToDigit(char c) { return c - '0'; }
    static bool willOverflow(integral_t value, integral_t digit);
    static std::string lexemeToKeyword(std::string_view lexeme);

    static const std::unordered_map<char, char> escapedChars_;
};

#endif
