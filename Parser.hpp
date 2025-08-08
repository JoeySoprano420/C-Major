#pragma once
#include "Token.hpp"
#include "AST.hpp"
#include <vector>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    ASTPtr parseProgram();

private:
    const std::vector<Token> ts;
    size_t i=0;

    const Token& peek() const;
    const Token& prev() const;
    bool check(TokenType t) const;
    bool match(std::initializer_list<TokenType> set);
    const Token& expect(TokenType t, const char* msg);

    // top-level
    ASTPtr program();
    ASTPtr capsule();
    ASTPtr func();

    // common
    ASTPtr block();             // ':' ... 'end'
    ASTPtr statement();
    ASTPtr letDecl();
    ASTPtr returnStmt();
    ASTPtr ifStmt();
    ASTPtr loopStmt();
    ASTPtr sayStmt();
    ASTPtr assignOrExprStmt();

    // expressions
    ASTPtr expression();
    ASTPtr parsePrecedence(int minPrec);
    ASTPtr primary();
    ASTPtr callPostfix(ASTPtr callee);

    int precedenceOf(const Token& t) const;  // *,/ > +,- > compares > && > ||
    bool isBinaryOp(TokenType t) const;
    bool isUnaryPrefix(TokenType t) const;
};
