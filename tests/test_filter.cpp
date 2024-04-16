#include <gtest/gtest.h>

#include "filter.hpp"
#include "types.hpp"

class FakeLexer : public ILexer {
    using TypeSequence = std::vector<Token::Type>;

   public:
    FakeLexer(std::initializer_list<Token::Type> typeSequence)
        : typeSequence_(typeSequence), current_(typeSequence_.begin()) {}

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

TEST(FilterTest, basic_filtering) {
    auto lexer = FakeLexer({Token::Type::SEMI, Token::Type::CMT, Token::Type::DOT});

    auto filter = Filter(&lexer, Token::Type::CMT);

    EXPECT_EQ(filter.getToken().getType(), Token::Type::SEMI);
    EXPECT_EQ(filter.getToken().getType(), Token::Type::DOT);
    EXPECT_EQ(filter.getToken().getType(), Token::Type::ETX);
}

TEST(FilterTest, forbid_etx_filtering) {
    auto lexer = FakeLexer({});
    EXPECT_THROW(Filter(&lexer, Token::Type::ETX), InvalidFilterType);
}
