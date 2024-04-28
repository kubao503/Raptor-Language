#include <fstream>
#include <iostream>

#include "base_errors.hpp"
#include "filter.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2)
        return -1;

    std::ifstream ifs(argv[1]);
    auto source = Source(ifs);
    auto lexer = Lexer(source);
    auto filter = Filter(lexer, Token::Type::CMT);
    auto parser = Parser(filter);

    Token token;
    try {
        do {
            token = filter.getToken();
            std::cout << token << '\n';
        } while (token.getType() != Token::Type::ETX);
    } catch (const BaseException& e) {
        std::cerr << '\n' << e.describe() << '\n';
    }
}
