#include "IRGen.hpp"
#include <stdexcept>

IRModule IRGen::generate(ASTPtr root){
    // implicit main if capsule 'main' exists: create wrapper calling it
    for (auto& n : root->kids){
        if (n->kind==ASTKind::Func){
            IRFunction f; f.name = n->name; mod.funcs.push_back(std::move(f));
        }
        if (n->kind==ASTKind::Capsule){
            IRFunction f; f.name = n->name; mod.funcs.push_back(std::move(f));
        }
    }
    // fill bodies
    for (auto& n : root->kids){
        for (auto& f : mod.funcs){
            if ((n->kind==ASTKind::Func || n->kind==ASTKind::Capsule) && f.name==n->name){
                cur = &f;
                // body is last child for func, all kids for capsule
                if (n->kind==ASTKind::Func){
                    auto body = n->kids.back();
                    genBlock(body);
                    cur->code.push_back({IROp::RET});
                } else {
                    for (auto& s : n->kids) genStmt(s);
                    cur->code.push_back({IROp::RET});
                }
            }
        }
    }
    // main wrapper if not present
    bool hasMain=false;
    for (auto& f : mod.funcs) if (f.name=="main") hasMain=true;
    if (!hasMain){
        IRFunction m; m.name="__entry";
        m.code.push_back({IROp::CALL,"main"});
        m.code.push_back({IROp::RET});
        mod.funcs.push_back(std::move(m));
    }
    return mod;
}

std::string IRGen::genExpr(ASTPtr e){
    switch (e->kind){
        case ASTKind::Literal: {
            auto t = newTmp();
            // naive: decide by first char
            if (!e->literal.empty() && std::isdigit((unsigned char)e->literal[0]))
                cur->code.push_back({IROp::ICONST,t,e->literal});
            else
                cur->code.push_back({IROp::SCONST,t,e->literal});
            return t;
        }
        case ASTKind::Var: {
            auto t=newTmp();
            cur->code.push_back({IROp::LOAD,t,e->name});
            return t;
        }
        case ASTKind::Unary: {
            auto r = genExpr(e->kids[0]);
            if (e->op=="-"){ auto t=newTmp(); cur->code.push_back({IROp::ICONST,t,"0"}); auto t2=newTmp(); cur->code.push_back({IROp::SUB,t2,t,r}); return t2; }
            if (e->op=="!"){ auto t=newTmp(); cur->code.push_back({IROp::NOT,t,r}); return t; }
            throw std::runtime_error("unary op not handled");
        }
        case ASTKind::Binary: {
            auto a = genExpr(e->kids[0]);
            auto b = genExpr(e->kids[1]);
            auto t = newTmp();
            if (e->op=="+") cur->code.push_back({IROp::ADD,t,a,b});
            else if (e->op=="-") cur->code.push_back({IROp::SUB,t,a,b});
            else if (e->op=="*") cur->code.push_back({IROp::MUL,t,a,b});
            else if (e->op=="/") cur->code.push_back({IROp::DIV,t,a,b});
            else if (e->op=="==") cur->code.push_back({IROp::CMP_EQ,t,a,b});
            else if (e->op!="!=" && e->op!="<" && e->op!="<=" && e->op!=">" && e->op!=">=")
                throw std::runtime_error("bin op not handled");
            else if (e->op=="!=") cur->code.push_back({IROp::CMP_NE,t,a,b});
            else if (e->op=="<")  cur->code.push_back({IROp::CMP_LT,t,a,b});
            else if (e->op=="<=") cur->code.push_back({IROp::CMP_LE,t,a,b});
            else if (e->op==">")  cur->code.push_back({IROp::CMP_GT,t,a,b});
            else if (e->op==">=") cur->code.push_back({IROp::CMP_GE,t,a,b});
            return t;
        }
        case ASTKind::Call: {
            // kids[0] = callee (Var or expr), kids[1..] args
            // emit args then CALL
            for (size_t k=1;k<e->kids.size();++k) (void)genExpr(e->kids[k]);
            cur->code.push_back({IROp::CALL, e->kids[0]->name});
            auto t=newTmp(); cur->code.push_back({IROp::ICONST,t,"0"}); // placeholder ret
            return t;
        }
        default: throw std::runtime_error("expr kind not supported");
    }
}

void IRGen::genStmt(ASTPtr s){
    switch (s->kind){
        case ASTKind::Let: {
            auto r = genExpr(s->kids[0]);
            cur->code.push_back({IROp::STORE,s->name,r});
            break;
        }
        case ASTKind::Assign: {
            auto r = genExpr(s->kids[0]);
            cur->code.push_back({IROp::STORE,s->name,r});
            break;
        }
        case ASTKind::Return: {
            if (!s->kids.empty()){
                auto r = genExpr(s->kids[0]);
                cur->code.push_back({IROp::LOAD,"_ret",r});
            }
            cur->code.push_back({IROp::RET});
            break;
        }
        case ASTKind::Say: {
            cur->code.push_back({IROp::PRINT,s->literal});
            break;
        }
        case ASTKind::If: {
            auto cond = genExpr(s->kids[0]);
            auto Lelse = newLbl();
            auto Lend  = newLbl();
            cur->code.push_back({IROp::JZ,cond,Lelse});
            genStmt(s->kids[1]); // then block
            cur->code.push_back({IROp::JMP,Lend});
            cur->code.push_back({IROp::LABEL,Lelse});
            if (s->kids.size()>2) genStmt(s->kids[2]);
            cur->code.push_back({IROp::LABEL,Lend});
            break;
        }
        case ASTKind::Loop: {
            auto Lbeg = newLbl(), Lend = newLbl();
            // s->name is loop var
            auto start = genExpr(s->kids[0]);
            cur->code.push_back({IROp::STORE,s->name,start});
            cur->code.push_back({IROp::LABEL,Lbeg});
            auto stop = genExpr(s->kids[1]);
            auto tmp = newTmp();
            cur->code.push_back({IROp::LOAD,tmp,s->name});
            auto cmp = newTmp();
            cur->code.push_back({IROp::CMP_LE,cmp,tmp,stop});
            cur->code.push_back({IROp::JZ,cmp,Lend});
            genStmt(s->kids[2]); // body
            // i = i + 1
            auto one=newTmp(); cur->code.push_back({IROp::ICONST,one,"1"});
            auto next=newTmp(); cur->code.push_back({IROp::ADD,next,tmp,one});
            cur->code.push_back({IROp::STORE,s->name,next});
            cur->code.push_back({IROp::JMP,Lbeg});
            cur->code.push_back({IROp::LABEL,Lend});
            break;
        }
        case ASTKind::Block: {
            genBlock(s);
            break;
        }
        default: {
            // Expression-as-statement
            if (s->kind==ASTKind::Binary || s->kind==ASTKind::Unary || s->kind==ASTKind::Call || s->kind==ASTKind::Literal || s->kind==ASTKind::Var)
                (void)genExpr(s);
            else
                throw std::runtime_error("stmt kind not handled");
        }
    }
}
void IRGen::genBlock(ASTPtr b){
    for (auto& k : b->kids) genStmt(k);
}
