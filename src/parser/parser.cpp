#include "parser.hpp"

#include "magic_enum/magic_enum.hpp"
#include "parser_errors.hpp"

std::optional<BuiltInType> getBuiltInType(const Token& token) {
    auto name = magic_enum::enum_name(token.getType());

    static constexpr std::size_t suffixSize{3};
    if (name.size() < suffixSize)
        return std::nullopt;

    name.remove_suffix(suffixSize);
    return magic_enum::enum_cast<BuiltInType>(name);
}

std::optional<Type> getType(const Token& token) {
    if (auto type = getBuiltInType(token))
        return type;
    if (token.getType() == Token::Type::ID)
        return std::get<std::string>(token.getValue());

    return std::nullopt;
}

/// PROGRAM = STMTS
Program Parser::parseProgram() {
    return {.statements = parseStatements()};
}

/// STMTS = { STMT }
Statements Parser::parseStatements() {
    Statements statements;
    while (auto statement = parseStatement()) statements.push_back(std::move(*statement));
    return statements;
}

/// STMT = IF_STMT
///      | WHILE_STMT
///      | RET_STMT
///      | PRINT_STMT
///      | CONST_VAR_DEF
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

    const auto expression = parseExpression();
    if (!expression)
        throw SyntaxException({}, "Expected expression (bool)");

    expect(Token::Type::L_C_BR, SyntaxException({}, "Missing left curly brace"));
    expect(Token::Type::R_C_BR, SyntaxException({}, "Missing right curly brace"));

    return IfStatement();
}

/// CONST_VAR_DEF = const ( TYPE | ID ) ID ASGN
std::optional<VarDef> Parser::parseConstVarDef() {
    if (currentToken_.getType() != Token::Type::CONST_KW)
        return std::nullopt;
    consumeToken();

    const auto type = getType(currentToken_);
    if (!type)
        throw SyntaxException({}, "Expected variable type");
    consumeToken();

    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID, SyntaxException({}, "Expected variable name"));

    auto assignment = parseAssignment(name);

    return VarDef{
        .isConst = true,
        .type = *type,
        .name = name,
        .expression = std::move(assignment->rhs),
    };
}

/// VOID_FUNC = void ID FUNC_DEF
std::optional<FuncDef> Parser::parseVoidFunc() {
    if (currentToken_.getType() != Token::Type::VOID_KW)
        return std::nullopt;
    consumeToken();

    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID, SyntaxException({}, "Expected function name"));

    return parseFuncDef(VoidType(), name);
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
    if (auto def = parseDef(name))
        return def;
    throw SyntaxException({}, "Expected assignment, field assignment or definition");
}

/// ASGN = '=' EXPR ';'
std::optional<Assignment> Parser::parseAssignment(const std::string& name) {
    if (currentToken_.getType() != Token::Type::ASGN_OP)
        return std::nullopt;

    consumeToken();

    auto expression = parseExpression();

    expect(Token::Type::SEMI, SyntaxException({}, "Missing semicolon"));

    return Assignment{.lhs = name, .rhs = std::move(*expression)};
}

/// BUILT_IN_DEF = TYPE DEF
std::optional<Statement> Parser::parseBuiltInDef() {
    const auto type = getBuiltInType(currentToken_);
    if (!type)
        return std::nullopt;

    consumeToken();
    return parseDef(*type);
}

/// DEF = ID ( FUNC_DEF
///          | ASGN )
std::optional<Statement> Parser::parseDef(const Type& type) {
    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID, SyntaxException({}, "Expected identifier"));

    const auto returnType = std::visit([](auto s) -> ReturnType { return s; }, type);
    if (auto def = parseFuncDef(returnType, name))
        return def;
    if (auto assignment = parseAssignment(name)) {
        return VarDef{.type = type,
                      .name = assignment->lhs,
                      .expression = std::move(assignment->rhs)};
    }
    throw SyntaxException({}, "Expected function or variable definition");
}

/// FUNC_DEF = '(' PARAMS ')' '{' STMTS '}'
std::optional<FuncDef> Parser::parseFuncDef(const ReturnType& returnType,
                                            const std::string& name) {
    if (currentToken_.getType() != Token::Type::L_PAR)
        return std::nullopt;
    consumeToken();

    const auto parameters = parseParameters();

    expect(Token::Type::R_PAR, SyntaxException({}, "Missing right parenthesis"));

    expect(Token::Type::L_C_BR, SyntaxException({}, "Missing left curly brace"));

    auto statements = parseStatements();

    expect(Token::Type::R_C_BR, SyntaxException({}, "Missing right curly brace"));
    return FuncDef(returnType, name, parameters, std::move(statements), {});
}

/// PARAMS = [ PARAM { ',' PARAM } ]
Parameters Parser::parseParameters() {
    Parameters parameters;

    auto parameter = parseParameter();
    if (!parameter)
        return parameters;

    parameters.push_back(*parameter);

    while (currentToken_.getType() == Token::Type::CMA) {
        consumeToken();
        parameter = parseParameter();
        if (!parameter)
            throw SyntaxException({}, "Expected parameter after comma");
        parameters.push_back(*parameter);
    }
    return parameters;
}

/// PARAM = [ ref ] TYPE ID
std::optional<Parameter> Parser::parseParameter() {
    const bool ref{currentToken_.getType() == Token::Type::REF_KW};
    if (ref)
        consumeToken();

    const auto type = getType(currentToken_);
    if (!type) {
        if (ref)
            throw SyntaxException({}, "Expected parameter type after ref keyword");
        return std::nullopt;
    }
    consumeToken();

    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID, SyntaxException({}, "Expected parameter name"));

    return Parameter{.type = *type, .name = name, .ref = ref};
}

/// EXPR = CONJ { or CONJ }
///      | '{' { EXPRS } '}'
std::optional<Expression> Parser::parseExpression() {
    auto leftLogicFactor = parseConjunctionExpression();
    if (!leftLogicFactor)
        return std::nullopt;

    while (currentToken_.getType() == Token::Type::OR_KW) {
        consumeToken();
        auto rightLogicFactor = parseConjunctionExpression();
        if (!rightLogicFactor)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected expression after 'or' keyword");
        leftLogicFactor =
            std::unique_ptr<DisjunctionExpression>(new DisjunctionExpression{
                .lhs = std::move(*leftLogicFactor), .rhs = std::move(*rightLogicFactor)});
    }

    return leftLogicFactor;
}

/// CONJ = EQ { and EQ }
std::optional<Expression> Parser::parseConjunctionExpression() {
    auto leftLogicFactor = parseEqualExpression();
    if (!leftLogicFactor)
        return std::nullopt;

    while (currentToken_.getType() == Token::Type::AND_KW) {
        consumeToken();
        auto rightLogicFactor = parseEqualExpression();
        if (!rightLogicFactor)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected expression after 'and' keyword");
        leftLogicFactor =
            std::unique_ptr<ConjunctionExpression>(new ConjunctionExpression{
                .lhs = std::move(*leftLogicFactor), .rhs = std::move(*rightLogicFactor)});
    }

    return leftLogicFactor;
}

/// EQ = REL [ '==' REL ]
///    | REL [ '!=' REL ]
std::optional<Expression> Parser::parseEqualExpression() {
    auto leftEqFactor = parseConstant();
    if (!leftEqFactor)
        return std::nullopt;

    if (currentToken_.getType() == Token::Type::EQ_OP) {
        consumeToken();
        auto rightEqFactor = parseConstant();
        if (!rightEqFactor)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected expression after '==' operator");
        leftEqFactor = std::unique_ptr<EqualExpression>(new EqualExpression{
            .lhs = std::move(*leftEqFactor), .rhs = std::move(*rightEqFactor)});
    }

    return leftEqFactor;
}

std::optional<Expression> Parser::parseConstant() {
    const auto value = currentToken_.getValue();
    consumeToken();
    return Constant{.value = value};
}

Parser::StatementParsers Parser::statementParsers_{
    [](Parser& p) { return p.parseIfStatement(); },
    [](Parser& p) { return p.parseConstVarDef(); },
    [](Parser& p) { return p.parseVoidFunc(); },
    [](Parser& p) { return p.parseDefOrAssignment(); },
    [](Parser& p) { return p.parseBuiltInDef(); },
};
