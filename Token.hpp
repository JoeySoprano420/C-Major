#pragma once
#include <string>

enum class TokenType {
    Identifier, Keyword, Number, Float, String,
    Operator, Punctuation, Symbol,
    OpenParen, CloseParen, OpenBrace, CloseBrace,
    Comma, Colon, Semicolon, Equals,
    EndOfFile, Unknown
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type, std::string lexeme, int line, int column)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) {}
};
