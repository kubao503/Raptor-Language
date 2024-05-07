#include "statements.hpp"

template <typename T>
auto getBinaryStmtCtor() {
    return [](auto lhs, auto rhs) { return T{std::move(lhs), std::move(rhs)}; };
}

template <typename T>
auto getUnaryStmtCtor() {
    return [](auto stmt) { return T{std::move(stmt)}; };
}

std::optional<IfOrWhileStatement::Ctor> IfOrWhileStatement::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::IF_KW:
            return getBinaryStmtCtor<IfStatement>();
        case Token::Type::WHILE_KW:
            return getBinaryStmtCtor<WhileStatement>();
        default:
            return std::nullopt;
    }
}

std::optional<ReturnOrPrintStatement::Ctor> ReturnOrPrintStatement::getCtor(
    Token::Type type) {
    switch (type) {
        case Token::Type::RETURN_KW:
            return getUnaryStmtCtor<ReturnStatement>();
        case Token::Type::PRINT_KW:
            return getUnaryStmtCtor<PrintStatement>();
        default:
            return std::nullopt;
    }
}
