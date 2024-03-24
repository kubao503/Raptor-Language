#include "errors.hpp"

const char* InvalidToken::what() const noexcept {
    message_ = getName() + " at " + std::to_string(position_.line) + ':' +
               std::to_string(position_.column) + '\n' + additionalInfo();

    return message_.c_str();
}

std::string NotTerminatedStrConst::additionalInfo() const { return "str: " + strConst_; }

std::string NonEscapableChar::additionalInfo() const {
    return "char: '" + std::string(1, c_) + '\'';
}

std::string IntOverflow::additionalInfo() const {
    auto max = std::numeric_limits<integral_t>::max();
    return std::to_string(value_) + " * 10 + " + std::to_string(digit_) + " > " +
           std::to_string(max);
}