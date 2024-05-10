#include "parser.hpp"

#include "magic_enum/magic_enum.hpp"

std::optional<BuiltInType> Parser::getCurrentTokenBuiltInType() const {
    auto name = magic_enum::enum_name(currentToken_.getType());

    static constexpr std::size_t suffixSize{3};
    if (name.size() < suffixSize)
        return std::nullopt;

    name.remove_suffix(suffixSize);
    return magic_enum::enum_cast<BuiltInType>(name);
}

std::optional<Type> Parser::getCurrentTokenType() const {
    if (currentToken_.getType() == Token::Type::ID)
        return std::get<std::string>(currentToken_.getValue());
    return getCurrentTokenBuiltInType();
}

ReturnType typeToReturnType(const Type& type) {
    return std::visit([](auto s) -> ReturnType { return s; }, type);
}

/// PROGRAM = STMTS
Program Parser::parseProgram() {
    auto statements = parseStatements();
    expectEndOfFile();
    return {.statements = std::move(statements)};
}

void Parser::expectEndOfFile() const {
    if (currentToken_.getType() != Token::Type::ETX)
        throw SyntaxException(currentToken_.getPosition(), "Unknown statement");
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
std::optional<Statement> Parser::parseIfStatement() {
    if (currentToken_.getType() != Token::Type::IF_KW)
        return std::nullopt;
    consumeToken();

    auto expression = parseExpression();
    if (!expression)
        throw SyntaxException(currentToken_.getPosition(),
                              "Expected if-statement condition");

    expect(Token::Type::L_C_BR,
           SyntaxException(currentToken_.getPosition(), "Missing left curly brace"));

    auto statements = parseStatements();

    expect(Token::Type::R_C_BR,
           SyntaxException(currentToken_.getPosition(), "Missing right curly brace"));

    return IfStatement{std::move(*expression), std::move(statements)};
}

/// WHILE_STMT = while EXPR '{' STMTS '}'
std::optional<Statement> Parser::parseWhileStatement() {
    if (currentToken_.getType() != Token::Type::WHILE_KW)
        return std::nullopt;
    consumeToken();

    auto expression = parseExpression();
    if (!expression)
        throw SyntaxException(currentToken_.getPosition(),
                              "Expected while-statement condition");

    expect(Token::Type::L_C_BR,
           SyntaxException(currentToken_.getPosition(), "Missing left curly brace"));

    auto statements = parseStatements();

    expect(Token::Type::R_C_BR,
           SyntaxException(currentToken_.getPosition(), "Missing right curly brace"));

    return WhileStatement{std::move(*expression), std::move(statements)};
}

/// RET_STMT = return [ EXPR ] ';'
std::optional<Statement> Parser::parseReturnStatement() {
    if (currentToken_.getType() != Token::Type::RETURN_KW)
        return std::nullopt;
    consumeToken();

    auto expression = parseExpression();

    expect(Token::Type::SEMI,
           SyntaxException(currentToken_.getPosition(),
                           "Missing semicolon after return statement"));

    return ReturnStatement{std::move(expression)};
}

/// PRINT_STMT = print [ EXPR ] ';'
std::optional<Statement> Parser::parsePrintStatement() {
    if (currentToken_.getType() != Token::Type::PRINT_KW)
        return std::nullopt;
    consumeToken();

    auto expression = parseExpression();

    expect(Token::Type::SEMI, SyntaxException(currentToken_.getPosition(),
                                              "Missing semicolon after print statement"));

    return PrintStatement{std::move(expression)};
}

/// CONST_VAR_DEF = const TYPE ID ASGN
std::optional<VarDef> Parser::parseConstVarDef() {
    if (currentToken_.getType() != Token::Type::CONST_KW)
        return std::nullopt;
    consumeToken();

    const auto type = getCurrentTokenType();
    if (!type)
        throw SyntaxException(currentToken_.getPosition(), "Expected variable type");
    consumeToken();

    auto name = expectAndReturnValue<std::string>(
        Token::Type::ID,
        SyntaxException(currentToken_.getPosition(), "Expected variable name"));

    auto assignment = parseAssignment(name);

    return VarDef{
        .isConst = true,
        .type = *type,
        .name = std::move(name),
        .expression = std::move(assignment.rhs),
    };
}

/// VOID_FUNC = void ID FUNC_DEF
std::optional<FuncDef> Parser::parseVoidFunc() {
    if (currentToken_.getType() != Token::Type::VOID_KW)
        return std::nullopt;
    consumeToken();

    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID,
        SyntaxException(currentToken_.getPosition(), "Expected function name"));

    return parseFuncDef(VoidType(), name);
}

/// DEF_OR_ASGN = ID ( FIELD_ASGN
///                  | DEF
///                  | FUNC_CALL ';' )
std::optional<Statement> Parser::parseDefOrAssignment() {
    if (currentToken_.getType() != Token::Type::ID)
        return std::nullopt;

    auto name = std::get<std::string>(currentToken_.getValue());
    consumeToken();

    if (auto def = parseDef(name))
        return def;
    if (auto funcCall = parseFuncCall(name)) {
        expect(Token::Type::SEMI,
               SyntaxException(currentToken_.getPosition(),
                               "Missing semicolon after function call"));
        return funcCall;
    }
    return parseFieldAssignment(name);
}

/// FIELD_ASGN = { '.' ID } ASGN
Assignment Parser::parseFieldAssignment(const std::string& name) {
    LValue lvalue{name};

    while (currentToken_.getType() == Token::Type::DOT) {
        consumeToken();

        auto field = expectAndReturnValue<std::string>(
            Token::Type::ID, SyntaxException(currentToken_.getPosition(),
                                             "Expected field name after dot operator"));

        lvalue = std::unique_ptr<FieldAccess>(
            new FieldAccess{.container = std::move(lvalue), .field = std::move(field)});
    }

    return parseAssignment(std::move(lvalue));
}

/// ASGN = '=' EXPR ';'
Assignment Parser::parseAssignment(LValue lvalue) {
    expect(Token::Type::ASGN_OP,
           SyntaxException(currentToken_.getPosition(), "Expected assignment operator"));

    auto expression = parseExpression();
    if (!expression)
        throw SyntaxException(currentToken_.getPosition(),
                              "Expected expression after assignment");

    expect(Token::Type::SEMI,
           SyntaxException(currentToken_.getPosition(), "Missing semicolon"));

    return Assignment{.lhs = std::move(lvalue), .rhs = std::move(*expression)};
}

/// BUILT_IN_DEF = BUILT_IN_TYPE DEF
std::optional<Statement> Parser::parseBuiltInDef() {
    const auto type = getCurrentTokenBuiltInType();
    if (!type)
        return std::nullopt;

    consumeToken();
    return parseDef(*type);
}

/// DEF = ID ( FUNC_DEF | ASGN )
std::optional<Statement> Parser::parseDef(const Type& type) {
    if (currentToken_.getType() != Token::Type::ID)
        return std::nullopt;
    const auto name = std::get<std::string>(currentToken_.getValue());
    consumeToken();

    const auto returnType = typeToReturnType(type);
    if (auto def = parseFuncDef(returnType, name))
        return def;
    auto assignment = parseAssignment(name);
    return VarDef{.type = type,
                  .name = std::get<std::string>(assignment.lhs),
                  .expression = std::move(assignment.rhs)};
}

/// FUNC_DEF = '(' PARAMS ')' '{' STMTS '}'
std::optional<FuncDef> Parser::parseFuncDef(const ReturnType& returnType,
                                            const std::string& name) {
    if (currentToken_.getType() != Token::Type::L_PAR)
        return std::nullopt;
    const auto position = currentToken_.getPosition();
    consumeToken();

    auto parameters = parseList<Parameter>(&Parser::parseParameter);

    expect(Token::Type::R_PAR,
           SyntaxException(currentToken_.getPosition(),
                           "Missing right parenthesis after function parameter list"));
    expect(Token::Type::L_C_BR,
           SyntaxException(currentToken_.getPosition(),
                           "Missing left curly brace before function body"));

    auto statements = parseStatements();

    expect(Token::Type::R_C_BR,
           SyntaxException(currentToken_.getPosition(),
                           "Missing right curly brace after function body"));
    return FuncDef(returnType, name, std::move(parameters), std::move(statements),
                   position);
}

/// PARAM = [ ref ] TYPE ID
std::optional<Parameter> Parser::parseParameter() {
    const bool ref{currentToken_.getType() == Token::Type::REF_KW};
    if (ref)
        consumeToken();

    const auto type = getCurrentTokenType();
    if (!type) {
        if (ref)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected parameter type after ref keyword");
        return std::nullopt;
    }
    consumeToken();

    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID,
        SyntaxException(currentToken_.getPosition(), "Expected parameter name"));

    return Parameter{.type = *type, .name = name, .ref = ref};
}

/// FUNC_CALL = '(' ARGS ')'
std::optional<FuncCall> Parser::parseFuncCall(const std::string& name) {
    if (currentToken_.getType() != Token::Type::L_PAR)
        return std::nullopt;
    consumeToken();

    auto arguments = parseList<Argument>(&Parser::parseArgument);

    expect(Token::Type::R_PAR,
           SyntaxException(currentToken_.getPosition(),
                           "Missing right parenthesis after function call arguments"));
    return FuncCall{.name = name, .arguments = std::move(arguments)};
}

/// STRUCT_DEF = struct ID '{' FIELDS '}'
std::optional<StructDef> Parser::parseStructDef() {
    if (currentToken_.getType() != Token::Type::STRUCT_KW)
        return std::nullopt;
    consumeToken();

    const auto name = expectAndReturnValue<std::string>(
        Token::Type::ID,
        SyntaxException(currentToken_.getPosition(), "Expected struct name"));

    expect(Token::Type::L_C_BR,
           SyntaxException(currentToken_.getPosition(),
                           "Missing left curly brace in struct difinition"));

    auto fields = parseList<Field>(&Parser::parseField);

    expect(Token::Type::R_C_BR,
           SyntaxException(currentToken_.getPosition(),
                           "Missing right curly brace in struct difinition"));
    return StructDef{.name = name, .fields = std::move(fields)};
}

/// VNT_DEF = variant ID '{' TYPES '}'
std::optional<VariantDef> Parser::parseVariantDef() {
    if (currentToken_.getType() != Token::Type::VARIANT_KW)
        return std::nullopt;
    consumeToken();

    auto name = expectAndReturnValue<std::string>(
        Token::Type::ID,
        SyntaxException(currentToken_.getPosition(), "Expected variant name"));

    expect(Token::Type::L_C_BR,
           SyntaxException(currentToken_.getPosition(),
                           "Missing left curly brace in variant difinition"));

    auto types = parseList<Type>(&Parser::parseType);

    expect(Token::Type::R_C_BR,
           SyntaxException(currentToken_.getPosition(),
                           "Missing right curly brace in variant difinition"));

    return VariantDef{std::move(name), std::move(types)};
}

std::optional<Type> Parser::parseType() {
    const auto type = getCurrentTokenType();
    if (!type)
        return std::nullopt;
    consumeToken();
    return type;
}

/// FIELD = TYPE ID
std::optional<Field> Parser::parseField() {
    auto type = getCurrentTokenType();
    if (!type)
        return std::nullopt;
    consumeToken();

    auto name = expectAndReturnValue<std::string>(
        Token::Type::ID,
        SyntaxException(currentToken_.getPosition(), "Expected field name"));

    return Field{.type = *type, .name = std::move(name)};
}

/// EXPR = DISJ | STRUCT_INIT
std::optional<Expression> Parser::parseExpression() {
    if (auto expr = parseStructInitExpression())
        return expr;
    return parseDisjunctionExpression();
}

/// STRUCT_INIT = '{' { EXPRS } '}'
std::optional<Expression> Parser::parseStructInitExpression() {
    if (currentToken_.getType() != Token::Type::L_C_BR)
        return std::nullopt;
    consumeToken();

    auto exprs = parseList<Expression>(&Parser::parseExpression);

    expect(
        Token::Type::R_C_BR,
        SyntaxException(currentToken_.getPosition(),
                        "Missing right curly brace at the end of struct initialization"));
    return StructInitExpression{.exprs = std::move(exprs)};
}

/// DISJ = CONJ { or CONJ }
std::optional<Expression> Parser::parseDisjunctionExpression() {
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
                std::move(*leftLogicFactor), std::move(*rightLogicFactor)});
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
                std::move(*leftLogicFactor), std::move(*rightLogicFactor)});
    }

    return leftLogicFactor;
}

/// EQ = REL [ '==' REL ]
///    | REL [ '!=' REL ]
std::optional<Expression> Parser::parseEqualExpression() {
    auto leftEqFactor = parseRelExpression();
    if (!leftEqFactor)
        return std::nullopt;

    if (const auto& ctor = ComparisonExpression::getCtor(currentToken_.getType())) {
        consumeToken();
        auto rightEqFactor = parseRelExpression();
        if (!rightEqFactor)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected expression after (not)equal operator");
        leftEqFactor = (*ctor)(std::move(*leftEqFactor), std::move(*rightEqFactor));
    }

    return leftEqFactor;
}

/// REL = ADD [ '<' ADD ]
///     | ADD [ '>' ADD ]
///     | ADD [ '<=' ADD ]
///     | ADD [ '>=' ADD ]
std::optional<Expression> Parser::parseRelExpression() {
    auto leftRelFactor = parseAdditiveExpression();
    if (!leftRelFactor)
        return std::nullopt;

    if (const auto& ctor = RelationExpression::getCtor(currentToken_.getType())) {
        consumeToken();
        auto rightRelFactor = parseAdditiveExpression();
        if (!rightRelFactor)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected expression after relation operator");
        leftRelFactor = (*ctor)(std::move(*leftRelFactor), std::move(*rightRelFactor));
    }

    return leftRelFactor;
}

/// ADD = TERM { '+' TERM }
///     | TERM { '-' TERM }
std::optional<Expression> Parser::parseAdditiveExpression() {
    auto leftTerm = parseMultiplicativeExpression();
    if (!leftTerm)
        return std::nullopt;

    while (const auto& ctor = AdditionExpression::getCtor(currentToken_.getType())) {
        consumeToken();
        auto rightTerm = parseMultiplicativeExpression();
        if (!rightTerm)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected expression after additive operator");
        leftTerm = (*ctor)(std::move(*leftTerm), std::move(*rightTerm));
    }

    return leftTerm;
}

/// TERM = FACTOR { '*' FACTOR }
///      | FACTOR { '/' FACTOR }
std::optional<Expression> Parser::parseMultiplicativeExpression() {
    auto leftFactor = parseNegationExpression();
    if (!leftFactor)
        return std::nullopt;

    while (const auto& ctor =
               MultiplicativeExpression::getCtor(currentToken_.getType())) {
        consumeToken();
        auto rightFactor = parseNegationExpression();
        if (!rightFactor)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected expression after multiplicative operator");
        leftFactor = (*ctor)(std::move(*leftFactor), std::move(*rightFactor));
    }

    return leftFactor;
}

/// FACTOR = [ '-' | not ] UNARY
std::optional<Expression> Parser::parseNegationExpression() {
    const auto& ctor = NegationExpression::getCtor(currentToken_.getType());
    if (ctor)
        consumeToken();

    auto expr = parseTypeExpression();
    if (ctor)
        return (*ctor)(std::move(*expr));
    return expr;
}

/// UNARY = SRC [ as TYPE ]
///       | SRC [ is TYPE ]
std::optional<Expression> Parser::parseTypeExpression() {
    auto expr = parseFieldAccessExpression();
    if (!expr)
        return std::nullopt;

    if (const auto& ctor = TypeExpression::getCtor(currentToken_.getType())) {
        consumeToken();

        auto type = getCurrentTokenType();
        consumeToken();
        if (!type)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected type after is/as keyword");

        expr = (*ctor)(std::move(*expr), *type);
    }
    return expr;
}

/// SRC = CNTNR { '.' ID }
std::optional<Expression> Parser::parseFieldAccessExpression() {
    auto expr = parseContainerExpression();
    if (!expr)
        return std::nullopt;

    while (currentToken_.getType() == Token::Type::DOT) {
        consumeToken();
        auto field = expectAndReturnValue<std::string>(
            Token::Type::ID, SyntaxException(currentToken_.getPosition(),
                                             "Expected field name after dot operator"));
        expr = std::unique_ptr<FieldAccessExpression>(new FieldAccessExpression{
            .expr = std::move(*expr), .field = std::move(field)});
    }

    return expr;
}

/// CNTNR = '(' EXPR ')'
///       | CONST
///       | CALL_OR_VAR
std::optional<Expression> Parser::parseContainerExpression() {
    if (auto expr = parseNestedExpression())
        return expr;
    if (auto expr = parseConstant())
        return expr;
    return parseVariableAccessOrFuncCall();
}

std::optional<Expression> Parser::parseNestedExpression() {
    if (currentToken_.getType() != Token::Type::L_PAR)
        return std::nullopt;
    consumeToken();

    auto expr = parseExpression();

    expect(Token::Type::R_PAR,
           SyntaxException(currentToken_.getPosition(),
                           "Expected right parenthesis after nested expression"));
    return expr;
}

std::optional<Expression> Parser::parseConstant() {
    if (!currentToken_.isConstant())
        return std::nullopt;

    const auto value = currentToken_.getValue();
    consumeToken();
    return Constant{.value = value};
}

/// CALL_OR_VAR = ID [ '(' ARGS ')' ]
std::optional<Expression> Parser::parseVariableAccessOrFuncCall() {
    if (currentToken_.getType() != Token::Type::ID)
        return std::nullopt;
    const auto name = std::get<std::string>(currentToken_.getValue());
    consumeToken();

    if (auto expr = parseFuncCall(name))
        return expr;
    return VariableAccess{.name = name};
}

/// ARG = [ ref ] EXPR
std::optional<Argument> Parser::parseArgument() {
    const bool ref{currentToken_.getType() == Token::Type::REF_KW};
    if (ref)
        consumeToken();

    auto expr = parseExpression();
    if (!expr) {
        if (ref)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected function call argument expression");
        return std::nullopt;
    }

    return Argument{.value = std::move(*expr), .ref = ref};
}

Parser::StatementParsers Parser::statementParsers_{
    [](Parser& p) { return p.parseIfStatement(); },
    [](Parser& p) { return p.parseWhileStatement(); },
    [](Parser& p) { return p.parseReturnStatement(); },
    [](Parser& p) { return p.parsePrintStatement(); },
    [](Parser& p) { return p.parseConstVarDef(); },
    [](Parser& p) { return p.parseVoidFunc(); },
    [](Parser& p) { return p.parseDefOrAssignment(); },
    [](Parser& p) { return p.parseBuiltInDef(); },
    [](Parser& p) { return p.parseStructDef(); },
    [](Parser& p) { return p.parseVariantDef(); },
};
