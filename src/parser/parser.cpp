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
    auto name = std::get<std::string>(value);

    if (auto def = parseFuncDef(name))
        return def;
    return std::nullopt;
}

/// FUNC_DEF = '(' PARAMS ')' '{' STMTS '}'
std::optional<FuncDef> Parser::parseFuncDef(const std::string& name) {
    expectAndReturnValue(Token::Type::L_PAR,
                         SyntaxException({}, "Missing left parenthesis"));
    return FuncDef(name);
}

Parser::StatementParsers Parser::statementParsers_{
    [](Parser& p) { return p.parseIfStatement(); },
    [](Parser& p) { return p.parseBuiltInDef(); },
};
