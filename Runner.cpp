#include "IR.hpp"
#include "Scheduler.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>

struct VMFunction {
    const IRFunction* ir{};
    std::unordered_map<std::string,int> locals;
};

class VM {
    const IRModule& mod;
    std::unordered_map<std::string, size_t> funcIndex;
public:
    VM(const IRModule& m):mod(m){
        for (size_t i=0;i<m.funcs.size();++i) funcIndex[m.funcs[i].name]=i;
    }

    void call(const std::string& name){
        auto it = funcIndex.find(name);
        if (it==funcIndex.end()){ std::cerr<<"Unknown func "<<name<<"\n"; return; }
        exec(mod.funcs[it->second]);
    }

    void exec(const IRFunction& f){
        // extremely naive interpreter for demo
        std::unordered_map<std::string,int> reg;
        std::unordered_map<std::string,size_t> labelToPc;
        for (size_t pc=0; pc<f.code.size(); ++pc)
            if (f.code[pc].op==IROp::LABEL) labelToPc[f.code[pc].a]=pc;

        for (size_t pc=0; pc<f.code.size(); ){
            const auto& ins = f.code[pc];
            switch (ins.op){
                case IROp::ICONST: reg[ins.a]=std::stoi(ins.b); pc++; break;
                case IROp::SCONST: reg[ins.a]=0; pc++; break; // ignore real strings here
                case IROp::LOAD:   reg[ins.a]=reg[ins.b]; pc++; break;
                case IROp::STORE:  reg[ins.a]=reg[ins.b]; pc++; break;
                case IROp::ADD:    reg[ins.a]=reg[ins.b]+reg[ins.c]; pc++; break;
                case IROp::SUB:    reg[ins.a]=reg[ins.b]-reg[ins.c]; pc++; break;
                case IROp::MUL:    reg[ins.a]=reg[ins.b]*reg[ins.c]; pc++; break;
                case IROp::DIV:    reg[ins.a]=reg[ins.b]/reg[ins.c]; pc++; break;
                case IROp::CMP_EQ: reg[ins.a]=(reg[ins.b]==reg[ins.c]); pc++; break;
                case IROp::CMP_LE: reg[ins.a]=(reg[ins.b]<=reg[ins.c]); pc++; break;
                case IROp::CMP_LT: reg[ins.a]=(reg[ins.b]< reg[ins.c]); pc++; break;
                case IROp::CMP_GT: reg[ins.a]=(reg[ins.b]> reg[ins.c]); pc++; break;
                case IROp::CMP_GE: reg[ins.a]=(reg[ins.b]>=reg[ins.c]); pc++; break;
                case IROp::PRINT:  std::cout << ins.a << "\n"; pc++; break;
                case IROp::CALL:   call(ins.a); pc++; break;
                case IROp::JMP:    pc = labelToPc[ins.a]; break;
                case IROp::JZ:     if (!reg[ins.a]) pc = labelToPc[ins.b]; else pc++; break;
                case IROp::LABEL:  pc++; break;
                case IROp::RET:    return;
                default: pc++; break;
            }
        }
    }
};
