#include <limits>

#include "interpreter_errors.hpp"
#include "lexer_errors.hpp"
#include "magic_enum/magic_enum.hpp"

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

InvalidToken::InvalidToken(const Position& position, char c)
    : BaseException(position, "Unknown token starting with '" + std::string(1, c) + '\''),
      c_(c) {}

NotTerminatedStrConst::NotTerminatedStrConst(const Position& position)
    : BaseException(position, "Encountered end of file while processing str literal") {}

NonEscapableChar::NonEscapableChar(const Position& position, char c)
    : BaseException(position, '\'' + std::string(1, c) + "' cannot be escaped with '\\'"),
      c_(c) {}

NumericOverflow::NumericOverflow(const Position& position, Integral value, Integral digit)
    : BaseException(position, "Detected overflow while constructing numeric literal\n"
                                  + std::to_string(value) + " * 10 + "
                                  + std::to_string(digit) + " > "
                                  + std::to_string(std::numeric_limits<Integral>::max())
                                  + " which is maximum value"),
      value_(value),
      digit_(digit) {}

InvalidFloat::InvalidFloat(const Position& position)
    : BaseException(position, "Expected digit after '.' in float literal") {}

struct TypeToString {
    std::string operator()(BuiltInType type) {
        return std::string(magic_enum::enum_name(type));
    }
    std::string operator()(const std::string& type) { return type; }
};

SymbolNotFound::SymbolNotFound(const Position& position, std::string type,
                               std::string symbol)
    : BaseException{position, type + " " + symbol + " not found"},
      type_{type},
      symbol_{symbol} {}

SymbolNotFound::SymbolNotFound(const Position& position, SymbolNotFound e)
    : SymbolNotFound{position, e.type_, e.symbol_} {}

TypeMismatch::TypeMismatch(const Position& position, Type expected, Type actual)
    : BaseException{position, "Expected: " + std::visit(TypeToString(), expected)
                                  + "\nActual:" + std::visit(TypeToString(), actual)},
      expected_{expected},
      actual_{actual} {}

TypeMismatch::TypeMismatch(const Position& position, TypeMismatch e)
    : TypeMismatch{position, e.expected_, e.actual_} {}
