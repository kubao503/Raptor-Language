#ifndef SOURCE_H
#define SOURCE_H

#include <istream>

struct Position {
    unsigned int line = 1;
    unsigned int column = 1;
};

class Source {
   public:
    Source(std::istream& stream)
        : stream_(stream) {
        nextChar();
    }

    char getChar() const { return currentChar_; }
    const Position& getPosition() { return currentPosition_; }
    void nextChar() {
        currentChar_ = stream_.get();
        currentPosition_.column += 1;

        if (currentChar_ == '\n') {
            currentPosition_.line += 1;
            currentPosition_.column = 1;
        }
    }

   private:
    std::istream& stream_;
    char currentChar_;
    Position currentPosition_;
};

#endif
