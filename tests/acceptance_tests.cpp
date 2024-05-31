#include <gtest/gtest.h>

#include "filter.hpp"
#include "interpreter.hpp"
#include "interpreter_errors.hpp"
#include "lexer.hpp"
#include "parser.hpp"

class AcceptanceTest : public testing::Test {
   protected:
    AcceptanceTest()
        : interpreter_{output_} {}

    void Init(const std::string& input) {
        stream_ = std::istringstream(input);
        source_ = std::make_unique<Source>(stream_);
        lexer_ = std::make_unique<Lexer>(*source_);
        filter_ = std::make_unique<Filter>(*lexer_, Token::Type::CMT);
        parser_ = std::make_unique<Parser>(*filter_);
        program_ = parser_->parseProgram();
    }

    std::string interpretAndGetOutput() {
        interpreter_.interpret(program_);
        return output_.str();
    }

    template <typename Exception>
    void interpretAndExpectThrowAt(Position position) {
        EXPECT_THROW(
            {
                try {
                    interpreter_.interpret(program_);
                } catch (const Exception& e) {
                    EXPECT_EQ(e.getPosition().line, position.line);
                    EXPECT_EQ(e.getPosition().column, position.column);
                    throw;
                }
            },
            Exception);
    }

    std::istringstream stream_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Filter> filter_;
    std::unique_ptr<Parser> parser_;
    Program program_;
    std::stringstream output_{};
    Interpreter interpreter_;
};

TEST_F(AcceptanceTest, data_types_and_operations) {
    Init(
        "bool b = not false or 1 == 1 and true != true;"
        "int i = 3 + 2 * 4.89 as int;"
        "float f = (2 as float) * (2.0 / 2 as float);"
        "print i;"
        "print f;"
        "print b;");
    EXPECT_EQ(interpretAndGetOutput(), "11\n2\ntrue\n");
}

TEST_F(AcceptanceTest, constants) {
    Init(
        "const float pi = 3.14;\n"
        "pi = 3;");
    interpretAndExpectThrowAt<ConstViolation>({2, 1});
}

TEST_F(AcceptanceTest, str_type) {
    Init(R"(str w = "Hello\n\"world\"";)"
         "print w;"
         R"(str v = "Hello" + " " + "wo";v = v + "rld";)"
         "print v;");
    EXPECT_EQ(interpretAndGetOutput(),
              "Hello\n"
              R"("world")"
              "\nHello world\n");
}

TEST_F(AcceptanceTest, comment) {
    Init("# print 22;");
    EXPECT_EQ(interpretAndGetOutput(), "");
}

TEST_F(AcceptanceTest, if_and_while_statement) {
    Init(
        "int i = 4;"
        "while i > 0 {"
        "    print i;"
        "    if i == 3 {"
        "        i = i - 1;"
        "    }"
        "    i = i - 1;"
        "}");
    EXPECT_EQ(interpretAndGetOutput(), "4\n3\n1\n");
}

TEST_F(AcceptanceTest, struct) {
    Init(
        "struct Point {"
        "    int x,"
        "    int y"
        "}"
        "Point p = {7, 2};"
        "p.y = 1;"
        "print p.y;"
        "p.y = p.x;"
        "print p.y;");
    EXPECT_EQ(interpretAndGetOutput(), "1\n7\n");
}

TEST_F(AcceptanceTest, functions) {
    Init(
        "int add_one(int num) {"
        "    return num + 1;"
        "}"
        "void add_one_ref(ref int num) {"
        "    num = num + 1;"
        "}"
        "void multi_parameter(int a, str b, bool c) {"
        "}"
        "int i = 3;"
        "int res = add_one(i);"
        "print res;"
        "add_one_ref(ref i);"
        "print i;");
    EXPECT_EQ(interpretAndGetOutput(), "4\n4\n");
}

TEST_F(AcceptanceTest, variant) {
    Init(
        "variant Number { int, float, str }"
        "void foo(Number n) {"
        "    if n is int {"
        "        int i = 2 * n as int;"
        "        print i;"
        "    }"
        "    if n is float {"
        "        float f = 0.5 * n as float;"
        "        print f;"
        "    }"
        "} Number a = 2.5 as Number;"
        "foo(a); a = 5 as Number; foo(a);");
    EXPECT_EQ(interpretAndGetOutput(), "1.25\n10\n");
}

TEST_F(AcceptanceTest, variant_with_structure) {
    Init(
        "struct Point {"
        "    int x,"
        "    int y"
        "}"
        "struct None {}"
        "variant Any {"
        "    Point,"
        "    None"
        "}"
        "Point p = {0, 1};"
        "Any a = p as Any;"
        "print (p as Point).y;");
    EXPECT_EQ(interpretAndGetOutput(), "1\n");
}

TEST_F(AcceptanceTest, variable_shadowning) {
    Init(
        "void foo() {"
        "    int i = 5;"
        "    print i;"
        "} int i = 3;"
        "print i;"
        "foo();");
    EXPECT_EQ(interpretAndGetOutput(), "3\n5\n");
}

TEST_F(AcceptanceTest, recursion) {
    Init(
        "void count_down_to_zero(int i) {"
        "    print i;"
        "    if i == 0 {"
        "        return;"
        "    }"
        "    count_down_to_zero(i - 1);"
        "}"
        "count_down_to_zero(3);");
    EXPECT_EQ(interpretAndGetOutput(), "3\n2\n1\n0\n");
}