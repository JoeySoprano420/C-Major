#include "EmitCIL.hpp"
#include <sstream>

static const char* cilOp(IROp op){
    switch(op){
        case IROp::ICONST: return "ldc.i4";
        case IROp::FCONST: return "ldc.r4";
        case IROp::SCONST: return "ldstr";
        case IROp::LOAD:   return "ldloc";
        case IROp::STORE:  return "stloc";
        case IROp::ADD:    return "add";
        case IROp::SUB:    return "sub";
        case IROp::MUL:    return "mul";
        case IROp::DIV:    return "div";
        case IROp::CMP_EQ: return "ceq";
        case IROp::CMP_NE: return "cne"; // pseudo
        case IROp::CMP_LT: return "clt";
        case IROp::CMP_LE: return "cle";
        case IROp::CMP_GT: return "cgt";
        case IROp::CMP_GE: return "cge";
        case IROp::AND:    return "and";
        case IROp::OR:     return "or";
        case IROp::NOT:    return "not";
        case IROp::JMP:    return "br";
        case IROp::JZ:     return "brfalse";
        case IROp::LABEL:  return "label";
        case IROp::CALL:   return "call";
        case IROp::RET:    return "ret";
        case IROp::PRINT:  return "call print";
        default:           return "nop";
    }
}

std::string emitCIL(const IRModule& m){
    std::ostringstream os;
    os << "// CIL-like output (illustrative)\n";
    for (auto& f : m.funcs){
        os << ".method static void " << f.name << "() {\n";
        for (auto& i : f.code){
            os << "  " << cilOp(i.op);
            if (!i.a.empty()) os << " " << i.a;
            if (!i.b.empty()) os << ", " << i.b;
            if (!i.c.empty()) os << ", " << i.c;
            os << "\n";
        }
        os << "}\n\n";
    }
    return os.str();
}
