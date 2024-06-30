#include <fstream>
#include <iostream>

#include "base_exception.hpp"
#include "filter.hpp"
#include "ir_generator.hpp"
#include "lexer.hpp"
#include "parser.hpp"

void compile(llvm::Module& module);

int main(int argc, char* argv[]) {
    if (argc < 2)
        return -1;

    std::ifstream ifs(argv[1]);
    auto source = Source(ifs);

    try {
        auto lexer = Lexer(source);
        auto filter = Filter(lexer, Token::Type::CMT);
        auto parser = Parser(filter);
        const auto program = parser.parseProgram();

        IRGenerator irGenerator;
        irGenerator.genIR(program);
        compile(*irGenerator.takeModule());

    } catch (const BaseException& e) {
        std::cerr << '\n' << e.describe() << '\n';
    }
}
