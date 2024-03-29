#ifndef LEXER_H
#define LEXER_H

#include <functional>
#include <optional>
#include <unordered_map>

#include "ILexer.hpp"
#include "source.hpp"
#include "token.hpp"

class Lexer : public ILexer {
    using builders_t = std::initializer_list<std::function<std::optional<Token>(Lexer&)>>;
    using chars_t = std::pair<char, char>;
    using token_types_t = std::pair<Token::Type, Token::Type>;

   public:
    Lexer(Source* source)
        : source_(source) {}

    Token getToken() override;

   private:
    Source* source_;
    Position tokenPosition_;

    void ignoreWhiteSpace() const;

    std::optional<Token> buildIdOrKeyword() const;
    std::optional<Token> buildKeyword(std::string_view lexeme) const;
    std::optional<Token> buildBoolConst(std::string_view lexeme) const;
    std::optional<Token> buildNumber() const;
    std::optional<Token> buildFloat(integral_t integralPart) const;
    std::optional<Token> buildStrConst() const;
    std::optional<Token> buildComment() const;
    std::optional<Token> buildNotEqualOp() const;
    std::optional<Token> buildOneLetterOp(char c, Token::Type type) const;
    std::optional<Token> buildTwoLetterOp(chars_t chars, token_types_t types) const;

    static integral_t charToDigit(char c) { return c - '0'; }
    static bool willOverflow(integral_t value, integral_t digit);
    static std::string lexemeToKeyword(std::string_view lexeme);

    static const builders_t builders_;
    static const std::unordered_map<char, char> escapedChars_;
};

#endif
