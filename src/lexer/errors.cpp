#include "errors.hpp"

#include <boost/core/demangle.hpp>

LexerException::LexerException(const Position& position, const std::string& message)
    : std::runtime_error(" at " + std::to_string(position.line) + ':'
                         + std::to_string(position.column) + '\n' + message),
      position_(position) {}

std::string LexerException::getName() const {
    auto name = typeid(*this).name();
    return boost::core::demangle(name);
}

std::string LexerException::describe() const {
    return getName() + what();
}

InvalidToken::InvalidToken(const Position& position, char c)
    : LexerException(position,
                     "Unknown token starting with '" + std::string(1, c) + '\''),
      c_(c) {}

NotTerminatedStrConst::NotTerminatedStrConst(const Position& position)
    : LexerException(position, "Encountered end of file while processing str literal") {}

NonEscapableChar::NonEscapableChar(const Position& position, char c)
    : LexerException(position,
                     '\'' + std::string(1, c) + "' cannot be escaped with '\\'"),
      c_(c) {}

NumericOverflow::NumericOverflow(const Position& position, Integral value, Integral digit)
    : LexerException(position, "Detected overflow while constructing numeric literal\n"
                                   + std::to_string(value) + " * 10 + "
                                   + std::to_string(digit) + " > "
                                   + std::to_string(std::numeric_limits<Integral>::max())
                                   + " which is maximum value"),
      value_(value),
      digit_(digit) {}

InvalidFloat::InvalidFloat(const Position& position)
    : LexerException(position, "Expected digit after '.' in float literal") {}
