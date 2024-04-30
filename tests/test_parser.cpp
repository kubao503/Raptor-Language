#include <gtest/gtest.h>

#include "fixtures.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "parser_errors.hpp"

class ParserTest : public testing::Test {
   protected:
    template <typename T>
    void SetUp(std::initializer_list<T> seq) {
        lexer_ = std::make_unique<FakeLexer>(seq);
        parser_ = std::make_unique<Parser>(*lexer_);
    }

    std::unique_ptr<FakeLexer> lexer_;
    std::unique_ptr<Parser> parser_;
};

TEST_F(ParserTest, parseProgram_empty) {
    SetUp({Token::Type::ETX});

    auto prog = parser_->parseProgram();
    EXPECT_EQ(prog.statements.size(), 0);
}

TEST_F(ParserTest, parseProgram_if_stmt_missing_left_par) {
    SetUp({Token::Type::IF_KW});

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(ParserTest, parseProgram_if_stmt) {
    SetUp({Token::Type::IF_KW, Token::Type::L_PAR});

    auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    EXPECT_TRUE(std::holds_alternative<IfStatement>(prog.statements.at(0)));
}

TEST_F(ParserTest, parse_func_def) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("foo"), {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    auto prog = parser_->parseProgram();
    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    auto funcDef = std::get<FuncDef>(prog.statements.at(0));
    EXPECT_EQ(funcDef.getName(), "foo");
    EXPECT_EQ(funcDef.getReturnType(), Token::Type::INT_KW);
    EXPECT_EQ(funcDef.getParameters().size(), 0);
}

TEST_F(ParserTest, parse_func_def_parameter) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("foo"), {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("num"), {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    auto funcDef = std::get<FuncDef>(prog.statements.at(0));
    ASSERT_EQ(funcDef.getParameters().size(), 1);

    auto param = funcDef.getParameters().at(0);
    EXPECT_EQ(param.type, Token::Type::INT_KW);
    EXPECT_EQ(param.name, "num");
    EXPECT_FALSE(param.ref);
}

TEST_F(ParserTest, parse_func_def_two_parameters) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("foo"), {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("num"), {}},
        {Token::Type::CMA, {}, {}},
        {Token::Type::BOOL_KW, {}, {}},
        {Token::Type::ID, std::string("truth"), {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    auto funcDef = std::get<FuncDef>(prog.statements.at(0));
    ASSERT_EQ(funcDef.getParameters().size(), 2);

    {
        auto param = funcDef.getParameters().at(0);
        EXPECT_EQ(param.type, Token::Type::INT_KW);
        EXPECT_EQ(param.name, "num");
        EXPECT_FALSE(param.ref);
    }

    auto param = funcDef.getParameters().at(1);
    EXPECT_EQ(param.type, Token::Type::BOOL_KW);
    EXPECT_EQ(param.name, "truth");
    EXPECT_FALSE(param.ref);
}

TEST_F(ParserTest, parse_func_def_no_parameter_after_comma) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("foo"), {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("num"), {}},
        {Token::Type::CMA, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(ParserTest, parse_func_ref_parameter) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("foo"), {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::REF_KW, {}, {}},
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("num"), {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    auto funcDef = std::get<FuncDef>(prog.statements.at(0));
    ASSERT_EQ(funcDef.getParameters().size(), 1);

    auto param = funcDef.getParameters().at(0);
    EXPECT_EQ(param.type, Token::Type::INT_KW);
    EXPECT_EQ(param.name, "num");
    EXPECT_TRUE(param.ref);
}

TEST_F(ParserTest, parse_func_no_parameter_after_ref) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("foo"), {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::REF_KW, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(ParserTest, parse_func_statements) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("foo"), {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::IF_KW, {}, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    auto funcDef = std::get<FuncDef>(prog.statements.at(0));
    auto statements = funcDef.getStatements();

    ASSERT_EQ(funcDef.getStatements().size(), 1);
    auto statement = funcDef.getStatements().at(0);
    ASSERT_TRUE(std::holds_alternative<IfStatement>(statement));
}

TEST_F(ParserTest, parse_void_func) {
    SetUp<Token>({
        {Token::Type::VOID_KW, {}, {}},
        {Token::Type::ID, std::string("foo"), {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<FuncDef>(prog.statements.at(0)));

    auto funcDef = std::get<FuncDef>(prog.statements.at(0));
    EXPECT_EQ(funcDef.getReturnType(), Token::Type::VOID_KW);
}

TEST_F(ParserTest, parse_void_func_no_name_after_void_kw) {
    SetUp<Token>({
        {Token::Type::VOID_KW, {}, {}},
        {Token::Type::L_PAR, {}, {}},
        {Token::Type::R_PAR, {}, {}},
        {Token::Type::L_C_BR, {}, {}},
        {Token::Type::R_C_BR, {}, {}},
    });

    EXPECT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(ParserTest, parse_assignment) {
    SetUp<Token>({
        {Token::Type::ID, std::string("var"), {}},
        {Token::Type::EQ_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
        {Token::Type::SEMI, {}, {}},
    });

    auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<Assignment>(prog.statements.at(0)));

    auto assignment = std::get<Assignment>(prog.statements.at(0));
    EXPECT_EQ(assignment.lhs, "var");
    ASSERT_TRUE(std::holds_alternative<Integral>(assignment.rhs));
    EXPECT_EQ(std::get<Integral>(assignment.rhs), 42);
}

TEST_F(ParserTest, parse_assignment_missing_semicolon) {
    SetUp<Token>({
        {Token::Type::ID, std::string("var"), {}},
        {Token::Type::EQ_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
    });

    ASSERT_THROW(parser_->parseProgram(), std::exception);
}

TEST_F(ParserTest, parse_var_def) {
    SetUp<Token>({
        {Token::Type::INT_KW, {}, {}},
        {Token::Type::ID, std::string("var"), {}},
        {Token::Type::EQ_OP, {}, {}},
        {Token::Type::INT_CONST, static_cast<Integral>(42), {}},
        {Token::Type::SEMI, {}, {}},
    });

    auto prog = parser_->parseProgram();

    ASSERT_EQ(prog.statements.size(), 1);
    ASSERT_TRUE(std::holds_alternative<VarDef>(prog.statements.at(0)));

    const auto varDef = std::get<VarDef>(prog.statements.at(0));
    EXPECT_EQ(varDef.name, "var");
    ASSERT_TRUE(std::holds_alternative<Integral>(varDef.value));
    EXPECT_EQ(std::get<Integral>(varDef.value), 42);
}
