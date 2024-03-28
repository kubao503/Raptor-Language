#include <gtest/gtest.h>

#include "filter.hpp"

using TypeSequence = std::vector<Token::Type>;

class FakeLexer : public ILexer {
   public:
    FakeLexer(TypeSequence typeSequence)
        : typeSequence_(std::move(typeSequence)), current_(typeSequence_.begin()) {}

    Token getToken() {
        if (current_ != typeSequence_.end()) {
            return {*current_++, {}, {}};
        } else {
            return {default_, {}, {}};
        }
    }

   private:
    TypeSequence typeSequence_;
    TypeSequence::const_iterator current_;

    static constexpr Token::Type default_ = Token::Type::ETX;
};

TEST(FilterTest, basic_filtering) {
    TypeSequence seq = {Token::Type::SEMI, Token::Type::CMT, Token::Type::DOT};
    auto lexer = FakeLexer(std::move(seq));

    auto filter = Filter(&lexer, Token::Type::CMT);

    EXPECT_EQ(filter.getToken().type, Token::Type::SEMI);
    EXPECT_EQ(filter.getToken().type, Token::Type::DOT);
    EXPECT_EQ(filter.getToken().type, Token::Type::ETX);
}

TEST(FilterTest, forbid_etx_filtering) {
    auto lexer = FakeLexer({});
    EXPECT_THROW(Filter(&lexer, Token::Type::ETX), InvalidFilterType);
}
