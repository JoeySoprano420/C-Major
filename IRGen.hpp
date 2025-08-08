#pragma once
#include "AST.hpp"
#include "IR.hpp"
#include <string>
#include <unordered_map>

struct IRGen {
    IRModule mod;
    IRFunction* cur = nullptr;
    int tmp = 0, lbl=0;

    std::string newTmp(){ return "%t"+std::to_string(tmp++); }
    std::string newLbl(){ return "L"+std::to_string(lbl++); }

    IRModule generate(ASTPtr root);

    // helpers
    std::string genExpr(ASTPtr e);
    void genStmt(ASTPtr s);
    void genBlock(ASTPtr b);
};

