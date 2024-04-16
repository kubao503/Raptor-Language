#ifndef LEXER_H
#define LEXER_H

#include <functional>
#include <optional>

#include "ILexer.hpp"
#include "source.hpp"
#include "token.hpp"

class Lexer : public ILexer {
    using CharPair = std::pair<char, char>;
    using TokenTypes = std::pair<Token::Type, Token::Type>;

    using TokenBuilders =
        std::initializer_list<std::function<std::optional<Token>(Lexer&)>>;
    using EscapedChars = std::initializer_list<CharPair>;

    using IntWithDigitCount = std::pair<Integral, unsigned int>;

   public:
    explicit Lexer(Source* source)
        : source_(source) {}

    Token getToken() override;

   private:
    Source* source_;
    Position tokenPosition_;

    void ignoreWhiteSpace() const;

    std::optional<Token> buildIdOrKeyword() const;
    std::optional<Token> buildKeyword(std::string_view lexeme) const;
    std::optional<Token> buildBoolConst(std::string_view lexeme) const;
    std::optional<Token> buildIntConst() const;
    std::optional<Token> buildFloatConst(Integral integralPart) const;
    std::optional<Token> buildStrConst() const;
    std::optional<Token> buildComment() const;
    std::optional<Token> buildNotEqualOp() const;
    std::optional<Token> buildOneLetterOp(char c, Token::Type type) const;
    std::optional<Token> buildTwoLetterOp(CharPair chars, TokenTypes types) const;

    std::optional<IntWithDigitCount> buildNumber() const;
    void expectNoEndOfFile() const;
    char findInEscapedChars(char searched) const;

    static TokenBuilders TokenBuilders_;
    static EscapedChars escapedChars_;
};

#endif
