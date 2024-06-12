#include <gtest/gtest.h>

#include "source.hpp"

class SourceTest : public testing::Test {
   protected:
    void Init(const std::string& input) {
        stream_ = std::istringstream(input);
        source_ = std::make_unique<Source>(stream_);
    }

    std::istringstream stream_;
    std::unique_ptr<Source> source_;
};

TEST_F(SourceTest, getPosition) {
    Init("abc");

    auto position = source_->getPosition();
    ASSERT_EQ(position.line, 1);
    ASSERT_EQ(position.column, 1);
}

TEST_F(SourceTest, getPosition_after_nextChar) {
    Init("abc");

    source_->nextChar();

    auto position = source_->getPosition();
    ASSERT_EQ(position.line, 1);
    ASSERT_EQ(position.column, 2);
}

TEST_F(SourceTest, getPosition_new_line_before_nextChar) {
    Init("\nabc");

    auto position = source_->getPosition();
    ASSERT_EQ(position.line, 1);
    ASSERT_EQ(position.column, 1);
}

TEST_F(SourceTest, getPosition_new_line_after_nextChar) {
    Init("\nabc");

    source_->nextChar();

    auto position = source_->getPosition();
    ASSERT_EQ(position.line, 2);
    ASSERT_EQ(position.column, 1);
}

TEST_F(SourceTest, getChar) {
    Init("abc");

    ASSERT_EQ(source_->getChar(), 'a');
}

TEST_F(SourceTest, getChar_after_nextChar) {
    Init("abc");

    source_->nextChar();

    ASSERT_EQ(source_->getChar(), 'b');
}
