#include "semantic_errors.hpp"

#include "magic_enum/magic_enum.hpp"

struct TypeToString {
    std::string operator()(BuiltInType type) {
        return std::string(magic_enum::enum_name(type));
    }
    std::string operator()(const std::string& type) { return type; }
    std::string operator()(const VoidType&) { return "VOID"; }
};

SymbolNotFound::SymbolNotFound(const Position& position, std::string type,
                               std::string symbol)
    : BaseException{position, type + " " + symbol + " not found"},
      type_{type},
      symbol_{symbol} {}

SymbolNotFound::SymbolNotFound(const Position& position, const SymbolNotFound& e)
    : SymbolNotFound{position, e.type_, e.symbol_} {}

TypeMismatch::TypeMismatch(const Position& position, Type expected, Type actual)
    : BaseException{position, "Expected: " + std::visit(TypeToString(), expected)
                                  + "\nActual: " + std::visit(TypeToString(), actual)},
      expected_{expected},
      actual_{actual} {}

TypeMismatch::TypeMismatch(const Position& position, const TypeMismatch& e)
    : TypeMismatch{position, e.expected_, e.actual_} {}

ReturnTypeMismatch::ReturnTypeMismatch(const Position& position, ReturnType expected,
                                       ReturnType actual)
    : BaseException{position, "Expected: " + std::visit(TypeToString(), expected)
                                  + "\nActual: " + std::visit(TypeToString(), actual)},
      expected_{expected},
      actual_{actual} {}

ReturnTypeMismatch::ReturnTypeMismatch(const Position& position,
                                       const ReturnTypeMismatch& e)
    : ReturnTypeMismatch{position, e.expected_, e.actual_} {}

InvalidFieldCount::InvalidFieldCount(const Position& position, std::size_t expected,
                                     std::size_t actual)
    : BaseException{position, "Expected " + std::to_string(expected) + " fields but "
                                  + std::to_string(actual) + " were given"},
      expected_{expected},
      actual_{actual} {}

InvalidFieldCount::InvalidFieldCount(const Position& position, const InvalidFieldCount& e)
    : InvalidFieldCount{position, e.expected_, e.actual_} {}

InvalidField::InvalidField(const Position& position, std::string_view fieldName)
    : BaseException{position, "Invalid struct's field name " + std::string(fieldName)},
      fieldName_{fieldName} {}

InvalidField::InvalidField(const Position& position, const InvalidField& e)
    : InvalidField{position, e.fieldName_} {}

Redefinition::Redefinition(const Position& position, std::string type, std::string name)
    : BaseException{position, "Redefinition of " + name + " " + type}, name_{name} {}

InvalidTypeConversion::InvalidTypeConversion(const Position& position, Type from, Type to)
    : BaseException{position, "Cannot convert from " + std::visit(TypeToString(), from)
                                  + " to " + std::visit(TypeToString(), to)},
      from_{std::move(from)},
      to_{std::move(to)} {}

InvalidTypeConversion::InvalidTypeConversion(const Position& position,
                                             InvalidTypeConversion e)
    : InvalidTypeConversion{position, std::move(e.from_), e.to_} {}