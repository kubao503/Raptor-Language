#ifndef SOURCE_H
#define SOURCE_H

#include <istream>

#include "position.hpp"

/// @brief Source of characters that keeps track of current character and its position
class Source {
   public:
    /// @brief Constructs a new Source
    ///
    /// Immediately reads the first character by calling nextChar()
    /// @param stream from which characters will be read
    explicit Source(std::istream& stream)
        : stream_(stream) {
        currentChar_ = stream_.get();
    }

    /// @brief Returns current character
    /// @return Current character
    char getChar() const { return currentChar_; }

    /// @brief Returns position of current character
    /// @return Position of current character
    const Position& getPosition() const { return currentPosition_; }

    /// @brief Reads next character and calculates its position
    void nextChar() {
        if (currentChar_ == '\n') {
            currentPosition_.line += 1;
            currentPosition_.column = 0;
        }

        currentChar_ = stream_.get();
        currentPosition_.column += 1;
    }

   private:
    std::istream& stream_;
    char currentChar_{};
    Position currentPosition_{1, 1};
};

#endif
