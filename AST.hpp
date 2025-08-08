#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>

enum class ASTKind {
    Program, Capsule,
    Func, Param, Block,
    Let, Assign, Return,
    If, Loop, // loop i from a to b
    Say,
    // expressions
    Binary, Unary, Call, Var, Literal
};

struct AST;
using ASTPtr = std::shared_ptr<AST>;

struct AST {
    ASTKind kind;
    std::string name;           // id / function name / capsule name
    std::string literal;        // string/number text
    std::vector<ASTPtr> kids;   // children
    // for operators / typing
    std::string op;             // "+", "-", "==", etc.

    // utility ctors
    static ASTPtr Node(ASTKind k){ auto n=std::make_shared<AST>(); n->kind=k; return n; }
    static ASTPtr Lit(const std::string& v){ auto n=Node(ASTKind::Literal); n->literal=v; return n; }
    static ASTPtr Var(const std::string& v){ auto n=Node(ASTKind::Var); n->name=v; return n; }
};
