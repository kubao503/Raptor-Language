#include "lexer_errors.hpp"

#include <limits>

InvalidToken::InvalidToken(const Position& position, char c)
    : BaseException(position,
                    "Unknown token starting with '" + std::string(1, c) + '\'') {}

NotTerminatedStrConst::NotTerminatedStrConst(const Position& position)
    : BaseException(position, "Encountered end of file while processing str literal") {}

NonEscapableChar::NonEscapableChar(const Position& position, char c)
    : BaseException(position,
                    '\'' + std::string(1, c) + "' cannot be escaped with '\\'") {}

NumericOverflow::NumericOverflow(const Position& position, Integral value, Integral digit)
    : BaseException(position, "Detected overflow while constructing numeric literal\n"
                                  + std::to_string(value) + " * 10 + "
                                  + std::to_string(digit) + " > "
                                  + std::to_string(std::numeric_limits<Integral>::max())
                                  + " which is maximum value") {}

InvalidFloat::InvalidFloat(const Position& position)
    : BaseException(position, "Expected digit after '.' in float literal") {}