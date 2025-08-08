#include "EmitHEX.hpp"
#include <sstream>
#include <iomanip>

// Tiny fake encoding mapping op -> 1 byte opcode; operands ascii-separated
static unsigned opByte(IROp op){
    switch(op){
        case IROp::ICONST: return 0x01; case IROp::FCONST: return 0x02; case IROp::SCONST: return 0x03;
        case IROp::LOAD:   return 0x10; case IROp::STORE:  return 0x11;
        case IROp::ADD:    return 0x20; case IROp::SUB:    return 0x21; case IROp::MUL: return 0x22; case IROp::DIV: return 0x23; case IROp::MOD: return 0x24;
        case IROp::CMP_EQ: return 0x30; case IROp::CMP_NE: return 0x31; case IROp::CMP_LT: return 0x32; case IROp::CMP_LE: return 0x33; case IROp::CMP_GT: return 0x34; case IROp::CMP_GE: return 0x35;
        case IROp::AND:    return 0x40; case IROp::OR:     return 0x41; case IROp::NOT: return 0x42;
        case IROp::JMP:    return 0x50; case IROp::JZ:     return 0x51; case IROp::LABEL: return 0x52;
        case IROp::CALL:   return 0x60; case IROp::RET:    return 0x61; case IROp::PRINT: return 0x70;
        default: return 0xFF;
    }
}

std::string emitHEX(const IRModule& m){
    std::ostringstream os;
    os << "; HEX OPCODE STREAM\n";
    for (auto& f : m.funcs){
        os << "; FUNC " << f.name << "\n";
        for (auto& ins : f.code){
            os << std::hex << std::setfill('0') << std::setw(2) << (int)opByte(ins.op) << " "
               << ins.a << " " << ins.b << " " << ins.c << "\n";
        }
    }
    return os.str();
}
