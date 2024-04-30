#ifndef PARSER_H
#define PARSER_H

#include <functional>
#include <optional>

#include "ILexer.hpp"
#include "program.hpp"
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
    void expect(Token::Type expected, const std::exception& exception);

    template <typename T>
    T expectAndReturnValue(Token::Type expected, const std::exception& exception);

    Statements parseStatements();
    std::optional<Statement> parseStatement();
    std::optional<IfStatement> parseIfStatement();
    std::optional<VarDef> parseConstVarDef();
    std::optional<FuncDef> parseVoidFunc();
    std::optional<Statement> parseDefOrAssignment();
    std::optional<Assignment> parseAssignment(const std::string& name);
    std::optional<Statement> parseBuiltInDef();
    std::optional<Statement> parseDef(Type type);
    std::optional<FuncDef> parseFuncDef(Type returnType, const std::string& name);
    Parameters parseParameters();
    std::optional<Parameter> parseParameter();

    using StatementParsers =
        std::initializer_list<std::function<std::optional<Statement>(Parser&)>>;
    static StatementParsers statementParsers_;

    ILexer& lexer_;
    Token currentToken_;
};

template <typename T>
T Parser::expectAndReturnValue(Token::Type expected, const std::exception& exception) {
    const auto value = currentToken_.getValue();
    expect(expected, exception);
    return std::get<T>(value);
}

#endif
