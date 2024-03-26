#include <fstream>
#include <iostream>

#include "filter.hpp"
#include "lexer.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2)
        return -1;

    std::ifstream ifs(argv[1]);
    auto source = Source(ifs);
    auto lexer = Lexer(source);
    auto filter = Filter(lexer, Token::Type::CMT);

    Token token;
    try {
        do {
            token = filter.getToken();
            std::cout << token << '\n';
        } while (token.type != Token::Type::ETX);
    } catch (const std::exception& e) {
        std::cerr << "\nerr: " << e.what() << '\n';
    }
}
