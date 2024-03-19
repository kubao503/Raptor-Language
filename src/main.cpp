#include <fstream>
#include <iostream>

#include "lexer.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2)
        return -1;

    std::ifstream ifs(argv[1]);
    auto lexer = Lexer(ifs);
    std::cout << static_cast<int>(lexer.getToken().type) << '\n';
}
