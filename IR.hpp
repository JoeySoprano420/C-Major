#pragma once
#include <string>
#include <vector>
#include <unordered_map>

enum class IROp {
    ICONST, FCONST, SCONST,
    LOAD, STORE,
    ADD, SUB, MUL, DIV, MOD,
    CMP_EQ, CMP_NE, CMP_LT, CMP_LE, CMP_GT, CMP_GE,
    AND, OR, NOT,
    JMP, JZ, LABEL,
    CALL, RET, PRINT
};

struct IRInst {
    IROp op;
    std::string a, b, c; // generic operands (regs, imm, labels, names)
};

struct IRFunction {
    std::string name;
    std::vector<IRInst> code;
};

struct IRModule {
    std::vector<IRFunction> funcs;
};
