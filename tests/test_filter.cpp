#include <gtest/gtest.h>

#include "filter.hpp"
#include "fixtures.hpp"

TEST(FilterTest, basic_filtering) {
    auto lexer = FakeLexer({Token::Type::SEMI, Token::Type::CMT, Token::Type::DOT});

    auto filter = Filter(lexer, Token::Type::CMT);

    EXPECT_EQ(filter.getToken().getType(), Token::Type::SEMI);
    EXPECT_EQ(filter.getToken().getType(), Token::Type::DOT);
    EXPECT_EQ(filter.getToken().getType(), Token::Type::ETX);
}

TEST(FilterTest, forbid_etx_filtering) {
    auto lexer = FakeLexer({Token::Type::ETX});
    EXPECT_THROW(Filter(lexer, Token::Type::ETX), InvalidFilterType);
}
