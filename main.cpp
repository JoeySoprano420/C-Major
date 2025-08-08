#include "Lexer.hpp"
#include "Parser.hpp"
#include "Runner.cpp"
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: cmajor <file.cmaj>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    std::stringstream buffer;
    buffer << file.rdbuf();

    Lexer lexer(buffer.str());
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto ast = parser.parse();

    execute(ast);
    return 0;
}
