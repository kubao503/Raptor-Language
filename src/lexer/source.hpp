#ifndef SOURCE_H
#define SOURCE_H

#include <istream>

#include "position.hpp"

class Source {
   public:
    Source(std::istream* stream)
        : stream_(stream) {
        nextChar();
    }

    char getChar() const { return currentChar_; }
    const Position& getPosition() const { return currentPosition_; }
    void nextChar() {
        currentChar_ = stream_->get();
        currentPosition_.column += 1;

        if (currentChar_ == '\n') {
            currentPosition_.line += 1;
            currentPosition_.column = 0;
        }
    }

   private:
    std::istream* stream_;
    char currentChar_;
    Position currentPosition_;
};

#endif
