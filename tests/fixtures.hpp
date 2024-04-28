#include <vector>

#include "ILexer.hpp"

/// @brief FakeLexer implementing the same interface as Lexer. Used for testing
class FakeLexer : public ILexer {
    using TypeSequence = std::vector<Token::Type>;

   public:
    /// @brief Constructs FakeLexer that returns toekns of subsequent types from the
    /// sequence on each getToken() call
    /// @param typeSequence
    FakeLexer(std::initializer_list<Token::Type> typeSequence)
        : typeSequence_(typeSequence), current_(typeSequence_.begin()) {}

    /// @brief Returns token of subsequent type from the sequence. When all token types
    /// are used returns the end-of-text token
    /// @return Token of type from the typeSequence
    Token getToken() {
        if (current_ != typeSequence_.end()) {
            return Token(*current_++, {}, {});
        } else {
            return Token(default_, {}, {});
        }
    }

   private:
    TypeSequence typeSequence_;
    TypeSequence::iterator current_;

    static constexpr Token::Type default_ = Token::Type::ETX;
};
