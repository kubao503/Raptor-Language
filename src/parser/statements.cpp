#include "statements.hpp"

std::optional<IfOrWhileStatement::Ctor> IfOrWhileStatement::getCtor(Token::Type type) {
    switch (type) {
        case Token::Type::IF_KW:
            return [](Expression expr, Statements statements) {
                return IfStatement{std::move(expr), std::move(statements)};
            };
        case Token::Type::WHILE_KW:
            return [](Expression expr, Statements statements) {
                return WhileStatement{std::move(expr), std::move(statements)};
            };
        default:
            return std::nullopt;
    }
}
