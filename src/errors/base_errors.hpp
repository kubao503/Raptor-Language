#ifndef BASE_ERRORS_H
#define BASE_ERRORS_H

#include <stdexcept>
#include <string>

#include "position.hpp"

/// @brief Abstract class for exceptions
class BaseException : public std::runtime_error {
   public:
    explicit BaseException(const Position& position, const std::string& message);

    /// @brief Provides std::string representation of exception
    /// @return std::string representation of exception
    std::string describe() const;
    const Position& getPosition() const { return position_; }

   private:
    Position position_;

    std::string getName() const;
    using std::runtime_error::what;
};

#endif
