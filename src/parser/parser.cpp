#include "parser.hpp"

#include "parser_errors.hpp"

std::initializer_list<Token::Type> builtInTypes{
    Token::Type::INT_KW,
    Token::Type::FLOAT_KW,
    Token::Type::BOOL_KW,
    Token::Type::STR_KW,
};

bool isBuiltInType(Token::Type type);

auto Parser::expectAndReturnValue(Token::Type expected, const std::exception& exception) {
    if (currentToken_.getType() != expected)
        throw exception;

    const auto value = currentToken_.getValue();
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
///      | CONST_VAR
///      | VOID_FUNC
///      | DEF_OR_ASGN
///      | BUILT_IN_DEF
///      | STRUCT_DEF
///      | VNT_DEF
std::optional<Statement> Parser::parseStatement() {
    for (const auto& parser : statementParsers_) {
        if (auto statement = parser(*this))
            return statement;
    }

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

/// BUILT_IN_DEF = TYPE DEF
std::optional<FuncDef> Parser::parseBuiltInDef() {
    if (!isBuiltInType(currentToken_.getType()))
        return std::nullopt;

    defType_ = currentToken_.getType();
    consumeToken();
    return parseDef();
}

bool isBuiltInType(Token::Type type) {
    return std::find(builtInTypes.begin(), builtInTypes.end(), type)
           != builtInTypes.end();
}

/// DEF = ID ( FUNC_DEF
///          | ASGN )
std::optional<FuncDef> Parser::parseDef() {
    auto value =
        expectAndReturnValue(Token::Type::ID, SyntaxException({}, "Expected identifier"));
    defName_ = std::get<std::string>(value);

    if (auto def = parseFuncDef())
        return def;
    return std::nullopt;
}

/// FUNC_DEF = '(' PARAMS ')' '{' STMTS '}'
std::optional<FuncDef> Parser::parseFuncDef() {
    expectAndReturnValue(Token::Type::L_PAR,
                         SyntaxException({}, "Missing left parenthesis"));

    const auto parameters = parseParameters();

    expectAndReturnValue(Token::Type::R_PAR,
                         SyntaxException({}, "Missing right parenthesis"));

    expectAndReturnValue(Token::Type::L_C_BR,
                         SyntaxException({}, "Missing left curly brace"));

    const auto statements = parseStatements();

    expectAndReturnValue(Token::Type::R_C_BR,
                         SyntaxException({}, "Missing right curly brace"));
    return FuncDef(defType_, defName_, parameters, statements, {});
}

/// PARAMS = [ PARAM { ',' PARAM } ]
Parameters Parser::parseParameters() {
    Parameters parameters;

    auto parameter = parseParameter();
    if (!parameter)
        return parameters;

    parameters.push_back(parameter.value());

    while (currentToken_.getType() == Token::Type::CMA) {
        consumeToken();
        parameter = parseParameter();
        if (!parameter)
            throw SyntaxException({}, "Expected parameter after comma");
        parameters.push_back(parameter.value());
    }
    return parameters;
}

/// PARAM = [ ref ] TYPE ID
std::optional<Parameter> Parser::parseParameter() {
    bool ref{false};
    if (currentToken_.getType() == Token::Type::REF_KW) {
        ref = true;
        consumeToken();
    } else if (!isBuiltInType(currentToken_.getType()))
        return std::nullopt;

    const auto type = currentToken_.getType();
    consumeToken();

    const auto value = expectAndReturnValue(
        Token::Type::ID, SyntaxException({}, "Expected parameter name"));
    const auto name = std::get<std::string>(value);

    return Parameter{.type = type, .name = name, .ref = ref};
}

Parser::StatementParsers Parser::statementParsers_{
    [](Parser& p) { return p.parseIfStatement(); },
    [](Parser& p) { return p.parseBuiltInDef(); },
};
