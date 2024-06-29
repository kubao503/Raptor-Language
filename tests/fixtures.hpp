#include <algorithm>
#include <vector>

#include "i_lexer.hpp"

/// @brief FakeLexer implementing the same interface as Lexer. Used for testing
class FakeLexer : public ILexer {
    using TypeSequence = std::vector<Token>;

   public:
    /// @brief Constructs FakeLexer that returns tokens of subsequent types from the
    /// sequence on each getToken() call. The value and position of tokens is set to
    /// default.
    /// @param typeSequence
    FakeLexer(std::initializer_list<Token::Type> typeSequence) {
        std::transform(typeSequence.begin(), typeSequence.end(),
                       std::back_inserter(tokenSequence_),
                       [](auto type) { return Token{type, {}, {}}; });
        current_ = tokenSequence_.begin();
    }

    /// @brief Constructs FakeLexer that returns subsequent token from sequence on
    /// each getToken() call
    /// @param tokenSequence
    FakeLexer(std::initializer_list<Token> tokenSequence)
        : tokenSequence_(tokenSequence), current_(tokenSequence_.begin()) {}

    /// @brief Returns token of subsequent type from the sequence. When all token types
    /// are used returns the end-of-text token
    /// @return Token of type from the typeSequence
    Token getToken() {
        if (current_ != tokenSequence_.end())
            return *current_++;
        return Token(default_, {}, {});
    }

   private:
    TypeSequence tokenSequence_;
    TypeSequence::iterator current_;

    static constexpr Token::Type default_ = Token::Type::ETX;
};
