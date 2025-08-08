#pragma once
#include "Token.hpp"
#include "AST.hpp"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ASTNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek() const;
    Token advance();
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& msg);

    std::shared_ptr<ASTNode> capsule();
    std::shared_ptr<ASTNode> statement();
};
