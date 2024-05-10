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

    const Token& getCurrentToken() { return currentToken_; }

   private:
    void consumeToken() { currentToken_ = lexer_.getToken(); };
    void expectEndOfFile() const;

    template <typename Exception>
    void expect(Token::Type expected, const Exception& exception);

    template <typename T, typename Exception>
    T expectAndReturnValue(Token::Type expected, const Exception& exception);

    std::optional<BuiltInType> getCurrentTokenBuiltInType() const;
    std::optional<Type> getCurrentTokenType() const;

    Statements parseStatements();
    std::optional<Statement> parseStatement();
    std::optional<Statement> parseIfStatement();
    std::optional<Statement> parseWhileStatement();
    std::optional<Statement> parseReturnStatement();
    std::optional<Statement> parsePrintStatement();
    std::optional<VarDef> parseConstVarDef();
    std::optional<FuncDef> parseVoidFunc();
    std::optional<Statement> parseDefOrAssignment();
    Assignment parseFieldAssignment(const std::string& name);
    Assignment parseAssignment(LValue lvalue);
    std::optional<Statement> parseBuiltInDef();
    std::optional<Statement> parseDef(const Type& type);
    std::optional<FuncDef> parseFuncDef(const ReturnType& returnType,
                                        const std::string& name);
    std::optional<Parameter> parseParameter();
    std::optional<FuncCall> parseFuncCall(const std::string& name);
    std::optional<StructDef> parseStructDef();
    std::optional<Field> parseField();
    std::optional<VariantDef> parseVariantDef();
    std::optional<Type> parseType();
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
    std::optional<Argument> parseArgument();

    template <typename T, typename ElementParser>
    std::vector<T> parseList(ElementParser elementParser);

    using StatementParsers =
        std::initializer_list<std::function<std::optional<Statement>(Parser&)>>;
    static StatementParsers statementParsers_;

    ILexer& lexer_;
    Token currentToken_;
};

#include "parser.tpp"

#endif
