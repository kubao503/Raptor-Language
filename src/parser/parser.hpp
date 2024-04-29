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
    auto expectAndReturnValue(Token::Type expected, const std::exception& exception);

    Statements parseStatements();
    std::optional<Statement> parseStatement();
    std::optional<IfStatement> parseIfStatement();
    std::optional<FuncDef> parseBuiltInDef();
    std::optional<FuncDef> parseDef();
    std::optional<FuncDef> parseFuncDef(const std::string& name);

    using StatementParsers =
        std::initializer_list<std::function<std::optional<Statement>(Parser&)>>;
    static StatementParsers statementParsers_;

    ILexer& lexer_;
    Token currentToken_;
};

#endif
