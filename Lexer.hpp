#pragma once
#include "Token.hpp"
#include <vector>

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos = 0;
    int line = 1, column = 1;

    char peek() const;
    char advance();
    bool match(char expected);
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string& lexeme);

    Token identifier();
    Token number();
    Token string();
};
