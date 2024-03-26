#include "errors.hpp"

#include <boost/core/demangle.hpp>

std::string LexerException::getName() const {
    auto name = typeid(*this).name();
    return boost::core::demangle(name);
}

const char* LexerException::what() const noexcept {
    message_ = getName() + " at " + std::to_string(position_.line) + ':'
               + std::to_string(position_.column) + '\n' + additionalInfo();

    return message_.c_str();
}

std::string InvalidToken::additionalInfo() const {
    return "Unknown token starting with '" + std::string(1, c_) + '\'';
}

std::string InvalidNotEqualOp::additionalInfo() const {
    return "Invalid != operator";
}

std::string NotTerminatedStrConst::additionalInfo() const {
    return "Encountered end of file while processing str literal";
}

std::string NonEscapableChar::additionalInfo() const {
    return '\'' + std::string(1, c_) + "' cannot be escaped with '\\'";
}

std::string NumericOverflow::additionalInfo() const {
    using std::to_string;

    auto max = std::numeric_limits<integral_t>::max();
    return "Detected overflow while constructing numeric literal\n" + to_string(value_)
           + " * 10 + " + to_string(digit_) + " > " + to_string(max)
           + " which is maximum value";
}

std::string InvalidFloat::additionalInfo() const {
    return "Expected digit after '.' in float literal";
}
