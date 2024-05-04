#ifndef PARSER_H
#define PARSER_H

#include <functional>
#include <optional>

#include "ILexer.hpp"
#include "parse_tree.hpp"
#include "parser_errors.hpp"
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
    std::optional<PrintStatement> parsePrintStatement();
    std::optional<VarDef> parseConstVarDef();
    std::optional<FuncDef> parseVoidFunc();
    std::optional<Statement> parseDefOrAssignment();
    Assignment parseFieldAssignment(const std::string& name);
    Assignment parseAssignment(Container&& container);
    std::optional<Statement> parseBuiltInDef();
    std::optional<Statement> parseDef(const Type& type);
    std::optional<FuncDef> parseFuncDef(const ReturnType& returnType,
                                        const std::string& name);
    std::optional<Parameter> parseParameter();
    std::optional<Expression> parseExpression();
    std::optional<Expression> parseStructInitExpression();
    std::optional<Expression> parseDisjunctionExpression();
    std::optional<Expression> parseConjunctionExpression();
    std::optional<Expression> parseEqualExpression();
    std::optional<Expression> parseRelExpression();
    std::optional<Expression> parseAdditiveExpression();
    std::optional<Expression> parseMultiplicativeExpression();
    std::optional<Expression> parseNegationExpression();
    std::optional<Expression> parseTypeExpression();
    std::optional<Expression> parseFieldAccessExpression();
    std::optional<Expression> parseContainerExpression();
    std::optional<Expression> parseNestedExpression();
    std::optional<Expression> parseConstant();
    std::optional<Expression> parseVariableAccessOrFuncCall();
    std::optional<Expression> parseFuncCallExpression(const std::string& name);
    std::optional<Argument> parseArgument();

    template <typename T, typename ElementParser>
    std::vector<T> parseList(ElementParser elementParser);

    using BinaryExprCtor = std::function<Expression(Expression, Expression)>;
    using UnaryExprCtor = std::function<Expression(Expression)>;
    using TypeExprCtor = std::function<Expression(Expression, Type)>;

    std::optional<BinaryExprCtor> getEqualExprCtor();
    std::optional<BinaryExprCtor> getRelExprCtor();
    std::optional<BinaryExprCtor> getAddExprCtor();
    std::optional<BinaryExprCtor> getMultExprCtor();
    std::optional<UnaryExprCtor> getNegationExprCtor();
    std::optional<TypeExprCtor> getTypeExprCtor();

    using StatementParsers =
        std::initializer_list<std::function<std::optional<Statement>(Parser&)>>;
    static StatementParsers statementParsers_;

    ILexer& lexer_;
    Token currentToken_;
};

#include "parser.tpp"

#endif
