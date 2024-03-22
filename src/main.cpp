#include <fstream>
#include <iostream>

#include "lexer.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2)
        return -1;

    std::ifstream ifs(argv[1]);
    auto source = Source(ifs);
    auto lexer = Lexer(source);

    Token token;
    do {
        token = lexer.getToken();
        std::cout << token;
    } while (token.type != Token::Type::ETX);
    std::cout << '\n';
}
