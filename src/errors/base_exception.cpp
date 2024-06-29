#include "base_exception.hpp"

BaseException::BaseException(const Position& position, const std::string& message)
    : std::runtime_error(" at " + std::to_string(position.line) + ':'
                         + std::to_string(position.column) + '\n' + message),
      position_(position) {}

std::string BaseException::getName() const {
    return typeid(*this).name();
}

std::string BaseException::describe() const {
    return getName() + what();
}
