#include "Parser.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token Parser::peek() const {
    return pos < tokens.size() ? tokens[pos] : tokens.back();
}

Token Parser::advance() {
    return pos < tokens.size() ? tokens[pos++] : tokens.back();
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type, const std::string& msg) {
    if (match(type)) return tokens[pos - 1];
    throw std::runtime_error("Parse error: " + msg);
}

std::shared_ptr<ASTNode> Parser::capsule() {
    expect(TokenType::Identifier, "expected capsule name");
    std::string name = tokens[pos - 1].lexeme;
    expect(TokenType::Colon, "expected ':' after capsule name");

    auto node = std::make_shared<ASTNode>(ASTNodeType::Capsule, name);
    while (!match(TokenType::EndOfFile)) {
        node->children.push_back(statement());
    }
    return node;
}

std::shared_ptr<ASTNode> Parser::statement() {
    Token token = advance();
    if (token.lexeme == "say") {
        Token str = expect(TokenType::String, "expected string after say");
        auto node = std::make_shared<ASTNode>(ASTNodeType::Say);
        node->value = str.lexeme;
        return node;
    } else if (token.lexeme == "let") {
        Token name = expect(TokenType::Identifier, "expected variable name");
        expect(TokenType::Equals, "expected '='");
        Token val = advance();
        auto node = std::make_shared<ASTNode>(ASTNodeType::Let, name.lexeme, val.lexeme);
        return node;
    }

    throw std::runtime_error("Unexpected token in statement: " + token.lexeme);
}

std::shared_ptr<ASTNode> Parser::parse() {
    return capsule();
}
