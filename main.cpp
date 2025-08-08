#include "Lexer.hpp"
#include "Parser.hpp"
#include "IRGen.hpp"
#include "EmitHEX.hpp"
#include "EmitCIL.hpp"
#include "Runner.cpp"  // VM and scheduler
#include <fstream>
#include <sstream>
#include <iostream>

int main(int argc, char** argv){
    if (argc<2){ std::cerr<<"Usage: cmajor <file.cmaj> [--hex] [--cil] [--run]\n"; return 1; }

    std::ifstream in(argv[1]); if(!in){ std::cerr<<"Cannot open "<<argv[1]<<"\n"; return 1; }
    std::stringstream buf; buf<<in.rdbuf();

    Lexer lx(buf.str()); auto toks = lx.tokenize();
    Parser ps(toks); auto ast = ps.parseProgram();

    IRGen gen; auto mod = gen.generate(ast);

    bool doHex=false, doCil=false, doRun=true;
    for (int i=2;i<argc;i++){
        std::string a=argv[i];
        if (a=="--hex") doHex=true;
        if (a=="--cil") doCil=true;
        if (a=="--no-run") doRun=false;
        if (a=="--run") doRun=true;
    }
    if (doHex) std::cout << emitHEX(mod) << "\n";
    if (doCil) std::cout << emitCIL(mod) << "\n";

    if (doRun){
        VM vm(mod);
        vm.call("main"); // run capsule/func named main
    }
    return 0;
}
