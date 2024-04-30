#include "parser.hpp"

#include "parser_errors.hpp"

std::initializer_list<Token::Type> builtInTypes{
    Token::Type::INT_KW,
    Token::Type::FLOAT_KW,
    Token::Type::BOOL_KW,
    Token::Type::STR_KW,
};

bool isBuiltInType(Token::Type type);

void Parser::expect(Token::Type expected, const std::exception& exception) {
    if (currentToken_.getType() != expected)
        throw exception;

    consumeToken();
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

    expect(Token::Type::L_PAR, SyntaxException({}, "Missing left parenthesis"));

    return IfStatement();
}

/// VOID_FUNC = void ID FUNC_DEF
std::optional<FuncDef> Parser::parseVoidFunc() {
    if (currentToken_.getType() != Token::Type::VOID_KW)
        return std::nullopt;
    consumeToken();

    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID, SyntaxException({}, "Expected function name"));

    return parseFuncDef(Token::Type::VOID_KW, name);
}

/// DEF_OR_ASGN = ID ( FIELD_ASGN
///                  | ASGN
///                  | DEF )
std::optional<Statement> Parser::parseDefOrAssignment() {
    if (currentToken_.getType() != Token::Type::ID)
        return std::nullopt;

    const auto value = currentToken_.getValue();
    const auto name = std::get<std::string>(value);
    consumeToken();

    if (auto assignment = parseAssignment(name))
        return assignment;
    throw SyntaxException({}, "Expected assignment, field assignment or definition");
}

/// ASGN = '=' EXPR ';'
std::optional<Assignment> Parser::parseAssignment(const std::string& name) {
    if (currentToken_.getType() != Token::Type::EQ_OP)
        return std::nullopt;

    consumeToken();

    const auto value = currentToken_.getValue();
    consumeToken();

    expect(Token::Type::SEMI, SyntaxException({}, "Missing semicolon"));

    return Assignment{.lhs = name, .rhs = value};
}

/// BUILT_IN_DEF = TYPE DEF
std::optional<Statement> Parser::parseBuiltInDef() {
    if (!isBuiltInType(currentToken_.getType()))
        return std::nullopt;

    const auto type = currentToken_.getType();
    consumeToken();
    return parseDef(type);
}

bool isBuiltInType(Token::Type type) {
    return std::find(builtInTypes.begin(), builtInTypes.end(), type)
           != builtInTypes.end();
}

/// DEF = ID ( FUNC_DEF
///          | ASGN )
std::optional<Statement> Parser::parseDef(Token::Type type) {
    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID, SyntaxException({}, "Expected identifier"));

    if (auto def = parseFuncDef(type, name))
        return def;
    if (auto assignment = parseAssignment(name)) {
        return VarDef{.name = assignment.value().lhs, .value = assignment.value().rhs};
    }
    throw SyntaxException({}, "Expected function or variable definition");
}

/// FUNC_DEF = '(' PARAMS ')' '{' STMTS '}'
std::optional<FuncDef> Parser::parseFuncDef(Token::Type returnType,
                                            const std::string& name) {
    if (currentToken_.getType() != Token::Type::L_PAR)
        return std::nullopt;
    consumeToken();

    const auto parameters = parseParameters();

    expect(Token::Type::R_PAR, SyntaxException({}, "Missing right parenthesis"));

    expect(Token::Type::L_C_BR, SyntaxException({}, "Missing left curly brace"));

    const auto statements = parseStatements();

    expect(Token::Type::R_C_BR, SyntaxException({}, "Missing right curly brace"));
    return FuncDef(returnType, name, parameters, statements, {});
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

    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID, SyntaxException({}, "Expected parameter name"));

    return Parameter{.type = type, .name = name, .ref = ref};
}

Parser::StatementParsers Parser::statementParsers_{
    [](Parser& p) { return p.parseIfStatement(); },
    [](Parser& p) { return p.parseVoidFunc(); },
    [](Parser& p) { return p.parseDefOrAssignment(); },
    [](Parser& p) { return p.parseBuiltInDef(); },
};
