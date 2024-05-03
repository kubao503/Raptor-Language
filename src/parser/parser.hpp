#ifndef PARSER_H
#define PARSER_H

#include <functional>
#include <optional>

#include "ILexer.hpp"
#include "parse_tree.hpp"
#include "token.hpp"

/// @brief Parser building parse tree from tokens
class Parser {
   public:
    explicit Parser(ILexer& lexer)
        : lexer_(lexer) {
        consumeToken();
    }

    /// @brief Builds parse tree from token acquired from lexer
    /// @return Parse tree
    Program parseProgram();

   private:
    void consumeToken() { currentToken_ = lexer_.getToken(); };

    template <typename Exception>
    void expect(Token::Type expected, const Exception& exception);

    template <typename T, typename Exception>
    T expectAndReturnValue(Token::Type expected, const Exception& exception);

    Statements parseStatements();
    std::optional<Statement> parseStatement();
    std::optional<IfStatement> parseIfStatement();
    std::optional<VarDef> parseConstVarDef();
    std::optional<FuncDef> parseVoidFunc();
    std::optional<Statement> parseDefOrAssignment();
    std::optional<Assignment> parseAssignment(const std::string& name);
    std::optional<Statement> parseBuiltInDef();
    std::optional<Statement> parseDef(const Type& type);
    std::optional<FuncDef> parseFuncDef(const ReturnType& returnType,
                                        const std::string& name);
    Parameters parseParameters();
    std::optional<Parameter> parseParameter();
    std::optional<Expression> parseExpression();
    std::optional<Expression> parseConjunctionExpression();
    std::optional<Expression> parseEqualExpression();
    std::optional<Expression> parseRelExpression();
    std::optional<Expression> parseAdditiveExpression();
    std::optional<Expression> parseMultiplicativeExpression();
    std::optional<Expression> parseConstant();

    using BinaryExprCtor =
        std::optional<std::function<Expression(Expression, Expression)>>;
    BinaryExprCtor getEqualExprCtor();
    BinaryExprCtor getRelExprCtor();
    BinaryExprCtor getAddExprCtor();
    BinaryExprCtor getMultExprCtor();

    using StatementParsers =
        std::initializer_list<std::function<std::optional<Statement>(Parser&)>>;
    static StatementParsers statementParsers_;

    ILexer& lexer_;
    Token currentToken_;
};

template <typename Exception>
void Parser::expect(Token::Type expected, const Exception& exception) {
    if (currentToken_.getType() != expected)
        throw exception;

    consumeToken();
}

template <typename T, typename Exception>
T Parser::expectAndReturnValue(Token::Type expected, const Exception& exception) {
    const auto value = currentToken_.getValue();
    expect(expected, exception);
    return std::get<T>(value);
}

#endif
