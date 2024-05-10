#ifndef LEXER_H
#define LEXER_H

#include <functional>
#include <optional>

#include "ILexer.hpp"
#include "source.hpp"
#include "token.hpp"

/// @brief Lexer that lazily converts characters read from source into tokens
class Lexer : public ILexer {
    using CharPair = std::pair<char, char>;
    using TokenTypes = std::pair<Token::Type, Token::Type>;

    using TokenBuilders =
        std::initializer_list<std::function<std::optional<Token>(Lexer&)>>;
    using EscapedChars = std::initializer_list<CharPair>;

    using IntWithDigitCount = std::pair<Integral, unsigned int>;

   public:
    /// @brief Constructs a Lexer that reads characters from the source
    /// @param source
    explicit Lexer(Source& source)
        : source_(source) {}

    /// @brief Returns next token lazily constructed from characters read from source
    /// @return Next token
    Token getToken() override;

   private:
    Source& source_;
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

    static TokenBuilders tokenBuilders_;
    static EscapedChars escapedChars_;
};

#endif
