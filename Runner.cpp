// vm_pure.cpp
// Self-contained, PURE C++ VM based on your snippet (no external headers).

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstddef>
#include <cstdlib>

// -----------------------------
// Minimal IR surface (inline):
// -----------------------------
enum class IROp {
    ICONST,   // a <- int(b)
    SCONST,   // a <- string(b)
    LOAD,     // a <- b
    STORE,    // a <- b (alias kept for clarity)
    ADD, SUB, MUL, DIV,
    CMP_EQ, CMP_LE, CMP_LT, CMP_GT, CMP_GE,
    PRINT,    // print a (int or string)
    CALL,     // call a(); if b != "", store return in b
    JMP,      // goto label a
    JZ,       // if !a goto b
    LABEL,    // define label a
    RET       // return a (optional)
};

struct IRInstr {
    IROp op{};
    std::string a, b, c; // simple 3-operand string fields for this toy IR
};

struct IRFunction {
    std::string name;
    std::vector<std::string> params;
    std::vector<IRInstr> code;
};

struct IRModule {
    std::vector<IRFunction> funcs;
};

// -----------------------------
// VM:
// -----------------------------
class VM {
public:
    explicit VM(const IRModule& m) : mod(m) {
        for (std::size_t i = 0; i < mod.funcs.size(); ++i) {
            funcIndex[mod.funcs[i].name] = i;
        }
    }

    // Call a function by name with optional integer args (positional).
    int call(const std::string& name, const std::vector<int>& args = {}) {
        auto it = funcIndex.find(name);
        if (it == funcIndex.end()) {
            std::cerr << "Unknown function: " << name << '\n';
            return 0;
        }
        return exec(mod.funcs[it->second], args);
    }

private:
    const IRModule& mod;
    std::unordered_map<std::string, std::size_t> funcIndex;

    static int toInt(const std::string& s) {
        // Accept decimal only (as per ICONST usage in original).
        return std::stoi(s);
    }

    static bool mapHas(const std::unordered_map<std::string, int>& m, const std::string& k) {
        return m.find(k) != m.end();
    }

    int exec(const IRFunction& f, const std::vector<int>& args) {
        // Integer registers and string registers
        std::unordered_map<std::string, int> reg;
        std::unordered_map<std::string, std::string> sreg;

        // Precompute label -> pc map
        std::unordered_map<std::string, std::size_t> labelToPc;
        for (std::size_t pc = 0; pc < f.code.size(); ++pc) {
            if (f.code[pc].op == IROp::LABEL) {
                labelToPc[f.code[pc].a] = pc;
            }
        }

        // Bind arguments to parameters
        for (std::size_t i = 0; i < f.params.size() && i < args.size(); ++i) {
            reg[f.params[i]] = static_cast<int>(args[i]);
        }

        int retVal = 0;

        for (std::size_t pc = 0; pc < f.code.size(); /* increment inside */) {
            const IRInstr& ins = f.code[pc];
            switch (ins.op) {
            case IROp::ICONST: {
                // reg[a] = int(b)
                reg[ins.a] = toInt(ins.b);
                ++pc;
            } break;

            case IROp::SCONST: {
                // sreg[a] = b
                sreg[ins.a] = ins.b;
                ++pc;
            } break;

            case IROp::LOAD: {
                // reg[a] = reg[b]
                reg[ins.a] = mapHas(reg, ins.b) ? reg[ins.b] : 0;
                ++pc;
            } break;

            case IROp::STORE: {
                // Alias of LOAD semantics (kept for clarity)
                reg[ins.a] = mapHas(reg, ins.b) ? reg[ins.b] : 0;
                ++pc;
            } break;

            case IROp::ADD: {
                reg[ins.a] = (mapHas(reg, ins.b) ? reg[ins.b] : 0)
                    + (mapHas(reg, ins.c) ? reg[ins.c] : 0);
                ++pc;
            } break;

            case IROp::SUB: {
                reg[ins.a] = (mapHas(reg, ins.b) ? reg[ins.b] : 0)
                    - (mapHas(reg, ins.c) ? reg[ins.c] : 0);
                ++pc;
            } break;

            case IROp::MUL: {
                reg[ins.a] = (mapHas(reg, ins.b) ? reg[ins.b] : 0)
                    * (mapHas(reg, ins.c) ? reg[ins.c] : 0);
                ++pc;
            } break;

            case IROp::DIV: {
                int rhs = mapHas(reg, ins.c) ? reg[ins.c] : 0;
                if (rhs == 0) {
                    std::cerr << "Division by zero\n";
                    return 0;
                }
                int lhs = mapHas(reg, ins.b) ? reg[ins.b] : 0;
                reg[ins.a] = lhs / rhs;
                ++pc;
            } break;

            case IROp::CMP_EQ: {
                reg[ins.a] = ((mapHas(reg, ins.b) ? reg[ins.b] : 0)
                    == (mapHas(reg, ins.c) ? reg[ins.c] : 0)) ? 1 : 0;
                ++pc;
            } break;

            case IROp::CMP_LE: {
                reg[ins.a] = ((mapHas(reg, ins.b) ? reg[ins.b] : 0)
                    <= (mapHas(reg, ins.c) ? reg[ins.c] : 0)) ? 1 : 0;
                ++pc;
            } break;

            case IROp::CMP_LT: {
                reg[ins.a] = ((mapHas(reg, ins.b) ? reg[ins.b] : 0)
                    < (mapHas(reg, ins.c) ? reg[ins.c] : 0)) ? 1 : 0;
                ++pc;
            } break;

            case IROp::CMP_GT: {
                reg[ins.a] = ((mapHas(reg, ins.b) ? reg[ins.b] : 0)
                    > (mapHas(reg, ins.c) ? reg[ins.c] : 0)) ? 1 : 0;
                ++pc;
            } break;

            case IROp::CMP_GE: {
                reg[ins.a] = ((mapHas(reg, ins.b) ? reg[ins.b] : 0)
                    >= (mapHas(reg, ins.c) ? reg[ins.c] : 0)) ? 1 : 0;
                ++pc;
            } break;

            case IROp::PRINT: {
                // Prefer int reg, then string reg, else raw operand
                if (mapHas(reg, ins.a)) {
                    std::cout << reg[ins.a] << std::endl;
                }
                else {
                    auto sit = sreg.find(ins.a);
                    if (sit != sreg.end()) {
                        std::cout << sit->second << std::endl;
                    }
                    else {
                        std::cout << ins.a << std::endl;
                    }
                }
                ++pc;
            } break;

            case IROp::CALL: {
                // Simple: call function in a; store result in b if not empty
                int result = call(ins.a);
                if (!ins.b.empty()) reg[ins.b] = result;
                ++pc;
            } break;

            case IROp::JMP: {
                auto it = labelToPc.find(ins.a);
                if (it == labelToPc.end()) {
                    std::cerr << "Unknown label: " << ins.a << '\n';
                    return 0;
                }
                pc = it->second;
            } break;

            case IROp::JZ: {
                int cond = mapHas(reg, ins.a) ? reg[ins.a] : 0;
                if (!cond) {
                    auto it = labelToPc.find(ins.b);
                    if (it == labelToPc.end()) {
                        std::cerr << "Unknown label: " << ins.b << '\n';
                        return 0;
                    }
                    pc = it->second;
                }
                else {
                    ++pc;
                }
            } break;

            case IROp::LABEL: {
                // No-op at runtime (labels pre-indexed)
                ++pc;
            } break;

            case IROp::RET: {
                if (!ins.a.empty() && mapHas(reg, ins.a)) {
                    retVal = reg[ins.a];
                }
                return retVal;
            } break;

            default: {
                // Unknown op: advance to avoid infinite loop
                ++pc;
            } break;
            }
        }
        return retVal;
    }
};

// ---------------------------------
// Tiny demo (remove if not needed):
// ---------------------------------
#ifdef VM_DEMO
static IRFunction makeHello() {
    IRFunction f;
    f.name = "hello";
    f.code = {
        { IROp::SCONST, "msg", "Hello from VM!", "" },
        { IROp::PRINT,  "msg", "", "" },
        { IROp::ICONST, "r", "42", "" },
        { IROp::RET,    "r", "", "" }
    };
    return f;
}

int main() {
    IRModule m;
    m.funcs.push_back(makeHello());
    VM vm(m);
    int r = vm.call("hello");
    std::cout << "Return: " << r << std::endl;
    return 0;
}
#endif
