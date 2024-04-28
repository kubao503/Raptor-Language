#include "parser.hpp"

#include "parser_errors.hpp"

auto Parser::expectAndReturnValue(Token::Type expected, const std::exception& exception) {
    if (currentToken_.getType() != expected)
        throw exception;

    auto value = currentToken_.getValue();
    consumeToken();
    return value;
}

/// PROGRAM = STMTS
Program Parser::parseProgram() {
    return {.statements = parseStatements()};
}

/// STMTS = { STMT }
Statements Parser::parseStatements() {
    Statements statements;
    while (auto statement = parseStatement()) statements.push_back(statement.value());
    return statements;
}

/// STMT = IF_STMT
///      | WHILE_STMT
///      | RET_STMT
///      | PRINT_STMT
///      | VAR_DECL
///      | ASGN
///      | FIELD_ASGN
///      | FUNC_DECL
///      | STRUCT_DECL
///      | VNT_DECL
std::optional<Statement> Parser::parseStatement() {
    if (auto statement = parseIfStatement())
        return statement;
    return std::nullopt;
}

/// IF_STMT = if EXPR '{' STMTS '}'
std::optional<IfStatement> Parser::parseIfStatement() {
    if (currentToken_.getType() != Token::Type::IF_KW)
        return std::nullopt;
    consumeToken();

    expectAndReturnValue(Token::Type::L_PAR,
                         SyntaxException({}, "Missing left parenthesis"));

    return IfStatement();
}
